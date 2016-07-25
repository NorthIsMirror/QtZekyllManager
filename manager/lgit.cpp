#include "lgit.h"
#include "singleton.h"
#include "messages.h"
#include "script_functions.h"

#include "git2/errors.h"

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

    retval =+ openRepo();
    if( retval > 0 ) {
        return retval + 1000000 * 17;
    }

    int error = git_reference_name_to_id(&oid, repo_, "HEAD");
    if( error == GIT_ENOTFOUND ) {
        MessagesI.AppendMessageT( "Cannot reset an empty repository" );
        retval += 47;
    } else if( error == GIT_EINVALIDSPEC ) {
        MessagesI.AppendMessageT( "Git backend (libgit2) has problems – it says \"HEAD\" isn't a valid spec");
        retval += 59;
    } else if( error < 0 ) {
        MessagesI.AppendMessageT( QString( "Git backend (libgit2) error (1) – \"%1\"" ).arg( decode_libgit2_error_code( error ) ) );
        retval += 67 + (100000 * error * -1);
    }
    if( error < 0 ) {
        closeRepo();
        return retval;
    }

    error = git_commit_lookup(&target, repo_, &oid);
    if( error < 0 ) {
        MessagesI.AppendMessageT( QString( "Git backend (libgit2) error (2) – \"%1\"" ).arg( decode_libgit2_error_code( error ) ) );
        retval += 73 + (100000 * error * -1);
        closeRepo();
        return retval;
    }

    error = git_reset( repo_, (git_object *) target, GIT_RESET_HARD, NULL );
    if( error < 0 ) {
        MessagesI.AppendMessageT( QString( "Git backend (libgit2) error (3) – \"%1\"" ).arg( decode_libgit2_error_code( error ) ) );
        retval += 83 + (100000 * error * -1);
        git_commit_free( target );
        closeRepo();
        return retval;
    }

    git_commit_free( target );
    closeRepo();
    return retval;
}

int lgit::openRepo()
{
    int retval = 0;
    int error = git_repository_open_ext( &repo_, repo_path_.toUtf8().constData(), 0, NULL );
    if( error < 0 ) {
        MessagesI.AppendMessageT( QString( "Error: Git backend problem (libgit2) when opening repository – \"%1\"" ).arg( decode_libgit2_error_code( error ) ) );
        retval += 89 + (100000 * error * -1);
    }

    return retval;
}

int lgit::closeRepo()
{
    git_repository_free( repo_ );
    repo_ = 0;
}
