#include "lgit_log.h"

#include "singleton.h"
#include "messages.h"

#include <string>

#include <QDebug>

#include "git2.h"
#include "git2/oid.h"
#include "git2/revwalk.h"
#include "git2/commit.h"

#define MessagesI Singleton<Messages>::instance()

static QDebug operator<<(QDebug out, const std::string & str)
{
    out << QString::fromStdString(str);
    return out;
}

lgit_log::lgit_log()
{

}


int lgit_log::log_of_tip( git_repository *repo, const std::string & sha, const std::string & hide )
{
    int error;
    git_revwalk *walk;
    git_oid oid, outid;
    git_object *obj;

    if ( ( error = git_oid_fromstr( &oid, sha.c_str() ) ) < 0 ) {
        MessagesI.AppendMessageT( "Invalid SHA given" );
        return 211 + ( 10000 * error * -1 );
    }

    if( hide != "" ) {
        if ( ( error = git_revparse_single(&obj, repo, hide.c_str() ) ) < 0 ) {
            MessagesI.AppendMessageT( "Invalid revision specification given (for hide)" );
            return 223 + ( 10000 * error * -1 );
        }
    }

    if ( ( error = git_revwalk_new( &walk, repo ) ) < 0 ) {
        MessagesI.AppendMessageT( "Unable to walk revisions" );
        return 227 + ( 10000 * error * -1 );
    }

    git_revwalk_push( walk, &oid );

    if( hide != "" ) {
        git_revwalk_hide( walk, git_object_id( obj ) );
    }

    int error2 = 0;
    while ( ( error2 = git_revwalk_next( &outid, walk ) ) == 0 ) {
        git_commit *commit;
        if ( ( error = git_commit_lookup( &commit, repo, &outid ) ) < 0 ) {
            MessagesI.AppendMessageT( "Unable to lookup commit" );
            continue;
        }

        log_entry newentry;

        // SHA
        char buf[ GIT_OID_HEXSZ + 1 ];
        const git_oid *commit_oid = git_commit_id( commit );
        if( commit_oid ) {
            git_oid_tostr( buf, sizeof(buf), commit_oid );
            newentry.sha = std::string( buf );
        }

        // AUTHOR
        const git_signature *sig;
        if( ( sig = git_commit_author( commit ) ) == 0 ) {
            MessagesI.AppendMessageT( "Unable to get commit's author" );
        } else {
            std::string name, email;
            if( sig->name ) {
                name = std::string( sig->name );
            }
            if( sig->email ) {
                email = std::string( " <" ) + std::string( sig->email ) + std::string( ">" );
            }
            newentry.author = name + email;
        }

        // MESSAGE
        const char *msg;
        if ( ( msg = git_commit_message( commit ) ) == 0 ) {
            MessagesI.AppendMessageT( "Unable to get commit's message" );
        } else {
            newentry.message = msg;
        }

        entries_.push_back( newentry );

        // qDebug() << newentry.sha << newentry.author << newentry.message;
    }

    if( error2 < 0 && error2 != GIT_ITEROVER ) {
        MessagesI.AppendMessageT( QString( "Problems during revision walk (%1)" ).arg( error2 ) );
    }

    git_revwalk_free( walk );

    return 0;
}
