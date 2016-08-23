/*
 Copyright 2016 Sebastian Gniazdowski

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "lgit_current.h"

#include "singleton.h"
#include "messages.h"

#include <string>

#include <QRegExp>
#include <QDebug>

#include "git2/reflog.h"

#define MessagesI Singleton<Messages>::instance()

lgit_current::lgit_current() : cur_repo_(0), state_( CURRENT_INVALID_EMPTY ), type_( CURRENT_TYPE_UNSET )
{

}

int lgit_current::discover( git_repository *repo )
{
    int error, retval = 0;
    const git_oid *oidp;
    git_oid oid;
    const char *sym;
    std::string symbolic_name;

    cur_repo_ = repo;

    clear();

    // Mark one branch as the checked out one
    git_reference *ref;
    if ( ( error = git_repository_head( &ref, repo ) ) < 0 ) {
        state_ = CURRENT_INVALID_MAIN;
        MessagesI.AppendMessageT( "Could not determine which branch is currently checked out" );
    } else {
        switch ( git_reference_type( ref ) ) {

        /* GIT_REF_OID */
        case GIT_REF_OID:
            // Prepare data for .tip_sha
            oidp = git_reference_target( ref );
            if ( !oidp ) {
                state_ = CURRENT_INVALID_OID;
                break;
            }
            state_ = CURRENT_OID;
            oid = *oidp;
            break;

        /* GIT_REF_SYMBOLIC */
        case GIT_REF_SYMBOLIC:
            sym = git_reference_symbolic_target( ref );
            if ( !sym ) {
                state_ = CURRENT_INVALID_SYM;
                break;
            }
            state_ = CURRENT_SYM;
            symbolic_name = std::string( sym );

            // Also establish OID
            if ( ( error = git_reference_name_to_id( &oid, repo, symbolic_name.c_str() ) ) < 0 ) {
                state_ = CURRENT_INVALID_OID2;
                break;
            }

            break;

        /* DEFAULT */
        /* GIT_REF_INVALID */
        /* GIT_REF_LISTALL */
        default:
            state_ = CURRENT_INVALID_OTHER;
            break;

        }

        /*
         * AFTER SWITCH()
         */

        if ( state_ == CURRENT_OID || state_ == CURRENT_SYM ) {
            char sha[ GIT_OID_HEXSZ + 1 ];
            git_oid_fmt( sha, &oid );
            sha[ GIT_OID_HEXSZ ] = '\0';
            current_oid_ = std::string( sha );
            type_ = CURRENT_TYPE_OID;
        }

        if ( state_ == CURRENT_OID ) {
            // Does the reference have any name?
            const char* refname_cstring = git_reference_name( ref );
            if( !refname_cstring ) {
                refname_cstring = "";
            }
            QString refname = QString::fromUtf8( refname_cstring ).trimmed();

            if( refname.isEmpty() || refname == "HEAD" ) {
                git_reflog *reflog;
                const git_reflog_entry *reflog_entry;

                if ( ( error = git_reflog_read( &reflog, repo, "HEAD" ) ) < 0 ) {
                    MessagesI.AppendMessageT( QString( "Could not read reflog, current HEAD will be established as SHA-only" ) +
                                              QString( " (no tag name resolution, error code: %1)" ) . arg( error ) );
                } else {
                    int count = git_reflog_entrycount( reflog );
                    int limit_count = 0;

                    for ( int i = 0; i < count; i ++ ) {
                        reflog_entry = git_reflog_entry_byindex( reflog, i );
                        if( !reflog_entry ) {
                            MessagesI.AppendMessageT( "Problems when reading reflog" );
                            continue;
                        }

                        oidp = git_reflog_entry_id_new( reflog_entry );
                        if( 0 == git_oid_cmp( oidp, &oid ) ) {
                            const char* message_cstring = git_reflog_entry_message( reflog_entry );
                            if( !message_cstring ) {
                                MessagesI.AppendMessageT( "Problems when reading reflog (2)" );
                                continue;
                            }

                            QString message = QString( message_cstring );
                            QRegExp rx("checkout: moving from.*to (.*)$");
                            if( rx.indexIn( message ) != -1 ) {
                                // The target of the catched move message must not be current HEAD's OID prefix
                                if( ! QString::fromStdString( current_oid_ ).startsWith( rx.cap( 1 ) ) ) {
                                    current_tag_ = rx.cap( 1 ).toStdString();
                                    type_ = CURRENT_TYPE_TAG;
                                }
                                break;
                            }

                            // Search only 5 OID-matching entries
                            if( ++ limit_count >= 5 ) {
                                break;
                            }
                        }
                    }

                    git_reflog_free( reflog );
                }
            } else {
                QStringList parts = refname.split( "/", QString::SkipEmptyParts );
                current_branch_ = parts.last().toStdString();
                type_ = CURRENT_TYPE_BRANCH;
            }
        } else if ( state_ == CURRENT_SYM ) {
            current_branch_ = symbolic_name;
        } else {
            MessagesI.AppendMessageT( QString( "Could not determine which branch is currently checked out (error code: %1)" ).arg( state_ ) );
        }

        git_reference_free( ref );
    }

    return retval;
}
