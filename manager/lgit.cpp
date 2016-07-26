#include <QDebug>
#include "git2/errors.h"

#include "lgit.h"
#include "singleton.h"
#include "messages.h"
#include "script_functions.h"

#define MessagesI Singleton<Messages>::instance()

#ifdef _WIN32
# include <Windows.h>
#else
# include <unistd.h>
#endif

lgit::lgit(QObject *parent) : QObject(parent) , repo_(NULL)
{
    git_libgit2_init();
}

lgit::~lgit()
{
    git_libgit2_shutdown();
}

int lgit::hardReset()
{
    int retval = 0;
    git_oid oid;
    git_commit *target;

    retval += openRepo();
    if( retval > 0 ) {
        MessagesI.AppendMessageT( "Could not open repository" + repo_path_ );
        return retval + 1000000 * 17;
    }

    int error = git_reference_name_to_id(&oid, repo_, "HEAD");
    if( error == GIT_ENOTFOUND ) {
        MessagesI.AppendMessageT( "Cannot reset an empty repository" );
        retval += 47;
    } else if( error == GIT_EINVALIDSPEC ) {
        MessagesI.AppendMessageT( "Git backend has problems – it says \"HEAD\" isn't a valid spec");
        retval += 59;
    } else if( error < 0 ) {
        MessagesI.AppendMessageT( QString( "Git backend error (1) – \"%1\"" ).arg( decode_libgit2_error_code( error ) ) );
        retval += 67 + (10000 * error * -1);
    }
    if( error < 0 ) {
        closeRepo();
        return retval;
    }

    error = git_commit_lookup(&target, repo_, &oid);
    if( error < 0 ) {
        MessagesI.AppendMessageT( QString( "Git backend error (2) – \"%1\"" ).arg( decode_libgit2_error_code( error ) ) );
        retval += 73 + (10000 * error * -1);
        closeRepo();
        return retval;
    }

    error = git_reset( repo_, (git_object *) target, GIT_RESET_HARD, NULL );
    if( error < 0 ) {
        MessagesI.AppendMessageT( QString( "Git backend error (3) – \"%1\"" ).arg( decode_libgit2_error_code( error ) ) );
        retval += 83 + (10000 * error * -1);
        git_commit_free( target );
        closeRepo();
        return retval;
    }

    git_commit_free( target );
    retval += closeRepo();
    return retval;
}

int lgit::commit( const QString & message )
{
    int retval = 0, error = 0;

    git_signature *sig;
    git_index *index;
    git_oid tree_id, parent_id, commit_id;
    git_tree *tree;
    git_commit *parent;

    retval += openRepo();
    if( retval > 0 ) {
        MessagesI.AppendMessageT( "Could not open repository" + repo_path_ );
        return retval + 10000 * 19;
    }

    if ( (error = git_signature_default( &sig, repo_ ) ) < 0 ) {
        retval += 89 + (10000 * error * -1);
        if ( error == GIT_ENOTFOUND ) {
            MessagesI.AppendMessageT( "Cannot commit: 'user.name' and 'user.email' must be set in a .gitconfig file" );
        }
        return retval;
    }

    if ( ( error = git_repository_index( &index, repo_ ) ) < 0 ) {
        MessagesI.AppendMessageT( "Cannot commit: could not open repository's index" );
        retval += 97 + (10000 * error * -1);
        git_signature_free( sig );
        retval += closeRepo();
        return retval;
    }

    if ( ( error = git_index_read( index, 1 ) ) < 0 ) {
        MessagesI.AppendMessageT( "Cannot commit: could not read already staged files" );
        retval += 101 + (10000 * error * -1);
        git_index_free( index );
        git_signature_free( sig );
        retval += closeRepo();
        return retval;
    }

    if ( ( error = git_index_write_tree( &tree_id, index ) ) < 0 ) {
        MessagesI.AppendMessageT( "Cannot commit: could not write initial tree from index" );
        retval += 103 + (10000 * error * -1);
        git_index_free( index );
        git_signature_free( sig );
        retval += closeRepo();
        return retval;
    }

    git_index_free( index );

    if ( ( error = git_tree_lookup( &tree, repo_, &tree_id ) ) < 0 ) {
        MessagesI.AppendMessageT( "Cannot commit: could not look up initial tree" );
        retval += 107 + (10000 * error * -1);
        git_signature_free( sig );
        retval += closeRepo();
        return retval;
    }

    if ( ( error = git_reference_name_to_id( &parent_id, repo_, "HEAD" ) ) < 0 ) {
        MessagesI.AppendMessageT( "Cannot commit: could not resolve what HEAD points to" );
        retval += 109 + (10000 * error * -1);
        git_tree_free( tree );
        git_signature_free( sig );
        retval += closeRepo();
        return retval;
    }

    if ( ( error = git_commit_lookup( &parent, repo_, &parent_id ) ) < 0 ) {
        MessagesI.AppendMessageT( "Cannot commit: could not resolve what HEAD points to (2)" );
        retval += 113 + (10000 * error * -1);
        git_tree_free( tree );
        git_signature_free( sig );
        retval += closeRepo();
        return retval;
    }

    if ( ( error = git_commit_create_v( &commit_id, repo_, "HEAD", sig, sig,
                                        NULL, message.toUtf8().constData(), tree, 1, parent ) ) < 0 ) {
        MessagesI.AppendMessageT( "Cannot commit: could not create the commit" );
        retval += 127 + (10000 * error * -1);
        git_commit_free( parent );
        git_tree_free( tree );
        git_signature_free( sig );
        retval += closeRepo();
        return retval;
    }

    git_commit_free( parent );
    git_tree_free( tree );
    git_signature_free( sig );
    retval += closeRepo();
    return retval;
}

int lgit::openRepo()
{
    int retval = 0;
    int error = git_repository_open_ext( &repo_, repo_path_.toUtf8().constData(), 0, NULL );
    if( error < 0 ) {
        MessagesI.AppendMessageT( QString( "Error: Git backend problem when opening repository – \"%1\"" ).arg( decode_libgit2_error_code( error ) ) );
        retval += 89 + (10000 * error * -1);
    }

    return retval;
}

int lgit::closeRepo()
{
    git_repository_free( repo_ );
    repo_ = 0;

    return 0;
}
