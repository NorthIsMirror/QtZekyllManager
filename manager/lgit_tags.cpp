#include "lgit_tags.h"

#include "singleton.h"
#include "messages.h"

#include <QString>
#include <QDebug>

#define MessagesI Singleton<Messages>::instance()

static int tag_foreach_callback( const char *name, git_oid *oid, void *payload );

static QDebug operator<<(QDebug out, const std::string & str)
{
    out << QString::fromStdString(str);
    return out;
}

struct tag_temp {
    std::string name;
    git_oid oid;

    tag_temp( const std::string & _name, const git_oid & _oid ) : name( _name ), oid( _oid )
    {}
};

lgit_tags::lgit_tags()
{

}

int lgit_tags::list( git_repository *repo )
{
    int error, retval = 0;

    std::vector< tag_temp > temp;
    git_tag_foreach( repo, tag_foreach_callback, &temp );

    char sha[ GIT_OID_HEXSZ + 1 ], tsha[ GIT_OID_HEXSZ + 1 ];
    sha[ GIT_OID_HEXSZ ] = '\0';
    tsha[ GIT_OID_HEXSZ ] = '\0';

    for ( std::vector< tag_temp >::iterator it = temp.begin(); it != temp.end(); ++ it ) {
        git_tag *tag;
        bool annotated = true;
        if ( ( error = git_tag_lookup( &tag, repo, &( it->oid ) ) ) < 0 ) {
            if( error == GIT_ENOTFOUND ) {
                annotated = false;
            } else {
                MessagesI.AppendMessageT( QString( "Cannot find a tag, the repo might be inconsistent (exit code: %1)" ).arg( error * -1 ) );
                retval += 443;
                continue;
            }
        }

        git_oid_fmt( sha, &( it->oid ) );
        sha[ GIT_OID_HEXSZ ] = '\0';

        mytag newtag;
        newtag.name = it->name;
        git_otype type = GIT_OBJ_ANY;

        if ( annotated ) {
            newtag.has_annotation = true;
            newtag.sha = std::string( sha );

            // Message
            const char *msg = git_tag_message( tag );
            if ( msg ) {
                newtag.message = std::string( msg );
            }

            // Tagger information
            const git_signature *sig = git_tag_tagger( tag );
            if ( sig ) {
                if ( sig->name ) {
                    newtag.user = std::string( sig->name );
                }
                if ( sig->email ) {
                    newtag.email = std::string( sig->email );
                }
            }

            // Target SHA
            git_object *targetObj;
            if ( ( error = git_tag_target( &targetObj, tag ) ) < 0 ) {
                MessagesI.AppendMessageT( QString( "Annotated tag '%1' points to non-existing commit (exit code: %2)" )
                                          .arg( it->name.c_str() ).arg( error * -1 ) );

                // Use tag-only (without object lookup) path to establish
                // target SHA and target type
                const git_oid *toid = git_tag_target_id( tag );
                if( toid ) {
                    git_oid_fmt( tsha, toid );
                    tsha[ GIT_OID_HEXSZ ] = '\0';
                    newtag.targetSha = std::string( tsha );
                }

                type = git_tag_target_type( tag );
            } else {
                newtag.targetExists = true;
                const git_oid *toid = git_object_id( targetObj );
                if( toid ) {
                    git_oid_fmt( tsha, toid );
                    tsha[ GIT_OID_HEXSZ ] = '\0';
                    newtag.targetSha = std::string( tsha );
                }

                type = git_object_type( targetObj );
                git_object_free( targetObj );
            }

            git_tag_free( tag );
        } else {
            newtag.targetSha = std::string( sha );
            newtag.has_annotation = false;
            git_object *targetObj;
            if ( ( error = git_object_lookup( &targetObj, repo, &( it->oid ), GIT_OBJ_ANY ) ) < 0 ) {
                MessagesI.AppendMessageT( QString( "Tag '%1' points to nonexisting commit %2 (exit code: %3)" )
                                          .arg( it->name.c_str() ).arg( sha ).arg( error * -1 ) );
                // Unknown type
                type = GIT_OBJ_ANY;
            } else {
                newtag.targetExists = true;
                type = git_object_type( targetObj );
                git_object_free( targetObj );
            }
        }

        switch( type ) {
        case GIT_OBJ_COMMIT:
            newtag.targetType = TAG_TARGET_COMMIT;
            break;
        case GIT_OBJ_TAG:
            newtag.targetType = TAG_TARGET_TAG;
            break;
        case GIT_OBJ_TREE:
            newtag.targetType = TAG_TARGET_TREE;
            break;
        case GIT_OBJ_ANY:
            newtag.targetType = TAG_TARGET_UNKNOWN;
            break;
        default:
            newtag.targetType = TAG_TARGET_OTHER;
            break;
        }

        // Remove "refs/tags/" from the tag name
        QStringList parts = QString::fromStdString( newtag.name ).split( "/", QString::SkipEmptyParts );
        if ( parts.count() != 3 ) {
            MessagesI.AppendMessageT( QString( "Unexpected tag reference occured: %1" ).arg( QString::fromStdString( newtag.name ) ) );
        }
        newtag.name = parts.last().toStdString();

        tags_.push_back( newtag );
    }

    return retval;
}

static int tag_foreach_callback( const char *name, git_oid *oid, void *payload ) {
    std::vector< tag_temp > *temp = static_cast< std::vector< tag_temp > * > ( payload );
    temp->push_back( tag_temp( std::string( name ), *oid ) );
    return 0;
}
