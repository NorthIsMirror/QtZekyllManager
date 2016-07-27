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

lgit::lgit(QObject *parent) : QObject(parent), constructor_error_code_(0), backend_status_(NOT_INITIALIZED),
                              is_name_set_(false), is_email_set_(false), is_when_set_(false), repo_(0)
{
    int error, retval = 0;

    if ( ( error = git_libgit2_init() ) < 0 ) {
        retval += 131 + (10000 * error * -1);
        backend_status_ = NOT_INITIALIZED;
        constructor_error_code_ = retval;
        return;
    }

    backend_status_ = PARTIAL_INIT_NO_DEFAULT_SIG;
}

lgit::~lgit()
{
    git_libgit2_shutdown();
}

std::tuple<QString, QString, git_time, int> lgit::testDefaultSignature()
{
    git_signature *sig;
    int error, retval = 0;
    QString name, email;
    git_time when;

    retval += openRepo();
    if( retval > 0 ) {
        retval += 1000000 * 23;
        return std::make_tuple( name, email, when, retval );
    }

    if ( (error = git_signature_default( &sig, repo_ ) ) < 0 ) {
        retval += 137 + (10000 * error * -1);
        return std::make_tuple( name, email, when, retval );
    }

    name = QString::fromUtf8( sig->name );
    email = QString::fromUtf8( sig->email );
    when = sig->when;

    if( backend_status_ == PARTIAL_INIT_NO_DEFAULT_SIG ) {
        backend_status_ = INITIALIZED;
    }

    git_signature_free( sig );

    retval += closeRepo();

    return std::make_tuple( name, email, when, retval );
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
        retval += 47 + (10000 * error * -1);
    } else if( error == GIT_EINVALIDSPEC ) {
        MessagesI.AppendMessageT( "Git backend has problems – it says \"HEAD\" isn't a valid spec");
        retval += 59 + (10000 * error * -1);
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

    git_signature *sig, *sig_default;
    git_index *index;
    git_oid tree_id, parent_id, commit_id;
    git_tree *tree;
    git_commit *parent;

    retval += openRepo();
    if( retval > 0 ) {
        MessagesI.AppendMessageT( "Could not open repository" + repo_path_ );
        return retval + 1000000 * 19;
    }

    if( !is_name_and_email_set() ) {
        if ( ( error = git_signature_default( &sig_default, repo_ ) ) < 0 ) {
            retval += 157 + (10000 * error * -1);
            if ( error == GIT_ENOTFOUND ) {
                MessagesI.AppendMessageT( "Cannot commit: 'user.name' and 'user.email' must be set in a gitconfig file, or provided in commit window" );
            } else {
                MessagesI.AppendMessageT( QString( "Could not access 'user.name' and 'user.email' from a gitconfig file, error code: %1" ).arg( retval ) );
            }
            retval += closeRepo();
            return retval;
        }

        QString name = QString::fromUtf8( sig_default->name ), email = QString::fromUtf8( sig_default->email );
        git_time mywhen = sig_default->when;

        git_signature_free( sig_default );

        if( is_name_set_ ) {
            name = name_;
        }
        if( is_email_set_ ) {
            email = email_;
        }
        if( is_when_set_ ) {
            mywhen = when_;
        }

        if( ( error = git_signature_new( &sig, name.toUtf8().constData(), email.toUtf8().constData(), mywhen.time, mywhen.offset ) ) < 0 ) {
            retval += 149 + (10000 * error * -1);
            MessagesI.AppendMessageT( QString( "Could not create commit's signature, are user.name and user.email correctly set in a gitconfig file" ) +
                                      QString( ", or provided in commit window? Error code: %1" ).arg( retval ) );
            retval += closeRepo();
            return retval;
        }
    } else {
        if( is_all_set() ) {
            if( ( error = git_signature_new( &sig, name_.toUtf8().constData(), email_.toUtf8().constData(), when_.time, when_.offset ) ) < 0 ) {
                retval += 139 + (10000 * error * -1);
                MessagesI.AppendMessageT( QString( "Could not create commit's signature, are user.name and user.email correctly set in a gitconfig file" ) +
                                          QString( ", or provided in commit window? Error code: %1" ).arg( retval ) );
                retval += closeRepo();
                return retval;
            }
        } else {
            if( ( error = git_signature_now( &sig, name_.toUtf8().constData(), email_.toUtf8().constData() ) ) < 0 ) {
                retval += 151 + (10000 * error * -1);
                MessagesI.AppendMessageT( QString( "Could not create commit's signature, are user.name and user.email correctly set in a gitconfig file" ) +
                                          QString( ", or provided in commit window? Error code: %1" ).arg( retval ) );
                retval += closeRepo();
                return retval;
            }
        }
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
        MessagesI.AppendMessageT( QString( "Git backend error when opening repository – \"%1\"" ).arg( decode_libgit2_error_code( error ) ) );
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
