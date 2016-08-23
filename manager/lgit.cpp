#include "lgit.h"

#include "singleton.h"
#include "messages.h"
#include "script_functions.h"

#include <QVector>
#include <QString>
#include <QWidget>
#include <QDebug>

#ifdef _WIN32
# include <Windows.h>
#else
# include <unistd.h>
#endif

#include "git2/annotated_commit.h"
#include "git2/repository.h"
#include "git2/checkout.h"
#include "git2/merge.h"
#include "git2/remote.h"

#define MessagesI Singleton<Messages>::instance()

static QDebug operator<<(QDebug out, const std::string & str)
{
    out << QString::fromStdString(str);
    return out;
}

static int fetch_transfer_progress_cb( const git_transfer_progress *stats, void *payload );
static int checkout_notify_cb( git_checkout_notify_t why, const char *path,
                               const git_diff_file *baseline, const git_diff_file *target, const git_diff_file *workdir,
                               void *payload );

lgit::lgit( QObject *parent ) : QObject( parent ), constructor_error_code_( 0 ), backend_status_( NOT_INITIALIZED ),
                              is_name_set_( false ), is_email_set_( false ), is_when_set_( false ), analysisResult_( ANALYSIS_UNSET ),
                              repo_( 0 )
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
        MessagesI.AppendMessageT( "Could not open repository " + repo_path_ + " (1)" );
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

int lgit::commit( const QString & message, const QVector< QString > & _parents )
{
    int retval = 0, error = 0;

    git_signature *sig, *sig_default;
    git_index *index;
    git_oid tree_id, commit_id;
    git_tree *tree;

    retval += openRepo();
    if( retval > 0 ) {
        MessagesI.AppendMessageT( "Could not open repository " + repo_path_ + " (2)" );
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

    // Prepare parents
    std::vector< git_oid > my_parent_oids;

    if( _parents.count() == 0 ) {
        git_oid my_parent_oid;

        if ( ( error = git_reference_name_to_id( &my_parent_oid, repo_, "HEAD" ) ) < 0 ) {
            MessagesI.AppendMessageT( "Cannot commit: could not resolve what HEAD points to" );
            retval += 109 + (10000 * error * -1);
            git_tree_free( tree );
            git_signature_free( sig );
            retval += closeRepo();
            return retval;
        }

        my_parent_oids.push_back( my_parent_oid );
    } else {
        git_oid my_parent_oid;
        unsigned int in_parents_count = _parents.count();

        for ( unsigned int i = 0; i < in_parents_count; ++ i ) {
            if ( ( error = git_oid_fromstr( &my_parent_oid, _parents[i].toUtf8().constData() ) ) < 0 ) {
                MessagesI.AppendMessageT( QString( "Cannot commit: could not parse parent's SHA (%1)" ).arg( _parents[i]) );
                retval += 347 + ( 10000 * error * -1 );
                git_tree_free( tree );
                git_signature_free( sig );
                retval += closeRepo();
                return retval;
            }

            my_parent_oids.push_back( my_parent_oid );
        }
    }

    // Harvest parent commits
    git_commit **gparents = new git_commit * [ my_parent_oids.size() ];
    int gparents_count = 0;

    for ( std::vector< git_oid >::iterator it = my_parent_oids.begin(); it != my_parent_oids.end(); ++ it ) {
        git_commit *gparent;
        if ( ( error = git_commit_lookup( &gparent, repo_, &(*it) ) ) < 0 ) {
            MessagesI.AppendMessageT( "Cannot commit: could not find a parent commit" );
            retval += 113 + (10000 * error * -1);
            git_tree_free( tree );
            git_signature_free( sig );

            for( int i = 0; i < gparents_count; ++ i ) {
                git_commit_free( gparents[i] );
            }
            delete [] gparents;

            retval += closeRepo();
            return retval;
        }

        ++ gparents_count;
        gparents[ gparents_count - 1 ] = gparent;
    }

    if ( ( error = git_commit_create( &commit_id, repo_, "HEAD", sig, sig,
                                        NULL, message.toUtf8().constData(), tree, gparents_count,
                                        const_cast< const git_commit **> ( gparents ) ) ) < 0 ) {
        const char *spec_error = giterr_last()->message;
        MessagesI.AppendMessageT( QString( "Cannot commit: %1" ).arg( QString::fromUtf8( spec_error ) ) );
        retval += 127 + (10000 * error * -1);
        git_tree_free( tree );
        git_signature_free( sig );

        for( int i = 0; i < gparents_count; ++ i ) {
            git_commit_free( gparents[i] );
        }
        delete [] gparents;

        retval += closeRepo();
        return retval;
    }

    git_tree_free( tree );
    git_signature_free( sig );

    for( int i = 0; i < gparents_count; ++ i ) {
        git_commit_free( gparents[i] );
    }
    delete [] gparents;

    retval += closeRepo();
    return retval;
}

int lgit::fetchBranch( const QString & branch , const QString & from ) {
    int error, retval = 0;

    git_remote *remote = 0;
    git_fetch_options fetch_opts;
    bool is_anonymous = false;

    if ( ( error = git_fetch_init_options( &fetch_opts, GIT_FETCH_OPTIONS_VERSION ) ) < 0 ) {
        retval += 167 + ( 10000 * error * -1 );
        MessagesI.AppendMessageT( QString( "Git backend error (4) – could not initialize fetch (%1)" ).arg( decode_libgit2_error_code(error) ) );
        return retval + 1000000 * 31;
    }

    error = openRepo();
    if ( error > 0 ) {
        retval += error;
        MessagesI.AppendMessageT( "Could not open repository " + repo_path_ + " (3)" );
        return retval + 1000000 * 29;
    }

    if ( ( error = git_remote_lookup( &remote, repo_, from.toUtf8().constData() ) ) < 0) {
        if ( ( error = git_remote_create_anonymous( &remote, repo_, from.toUtf8().constData() ) ) < 0) {
            retval += 163 + ( 100000000 * error * -1 );
            MessagesI.AppendMessageT( "Incorrect repo name or URL given" );
            retval += closeRepo();
            return retval;
        } else {
            is_anonymous = true;
        }
    }

    fetch_opts.callbacks.transfer_progress = fetch_transfer_progress_cb;
    fetch_opts.callbacks.payload = op_tracker_;

    QString refspec;
    git_strarray refspecs;
    char *refspec_cstr = 0;

    if( is_anonymous ) {
        // Only FETCH_HEAD will be updated/set up
        refspec = branch.trimmed();
    } else {
        if( ! branch.trimmed().isEmpty() ) {
            refspec = QString( "refs/heads/%1:refs/remotes/%2/%1" ) . arg( branch.trimmed() ) . arg( from.trimmed() );
        }
    }

    // Setup the "follow-config" fetch
    refspecs.count = 0;
    refspecs.strings = 0;

    // Is there specific branch selected? I.e. no "follow-config" fetch?
    if( ! refspec.trimmed().isEmpty() ) {
        refspec_cstr = create_cstring( refspec.trimmed().toUtf8().constData() );
        refspecs.count = 1;
        refspecs.strings = &refspec_cstr;
    }

    if ( ( error = git_remote_fetch( remote, &refspecs, &fetch_opts, NULL ) ) < 0 ) {
        retval += 181 + ( 10000 * error * -1 );
        MessagesI.AppendMessageT( QString( "Git backend error (5) – could not perform fetch (%1)" ).arg( decode_libgit2_error_code( error ) ) );
        if ( refspec_cstr ) {
            delete [] refspec_cstr;
        }
        return retval;
    }

    if( refspec_cstr ) {
        delete [] refspec_cstr;
        refspec_cstr = 0;
    }

    // List all branches, also with FETCH_HEAD information
    git_branches_.list( repo_, BRANCH_ALL );

    git_current_.discover( repo_ );

    retval += closeRepo();
    return retval;
}

int lgit::analyzeMerge( const std::string &, const std::string & tip_sha )
{
    int error, retval = 0;

    analysisResult_ = ANALYSIS_UNSET;

    error = openRepo();
    if ( error > 0 ) {
        retval += error;
        MessagesI.AppendMessageT( "Could not open repository " + repo_path_ + " (4)" );
        analysisResult_ = ANALYSIS_ERROR;
        return retval + 1000000 * 37;
    }

    git_oid their_oids[1];
    git_annotated_commit *their_heads[1];

    if ( ( error = git_oid_fromstr( &their_oids[0], tip_sha.c_str() ) ) < 0 ) {
        MessagesI.AppendMessageT( tr( "FETCH_HEAD contained improper SHA, cannot merge" ) );
        analysisResult_ = ANALYSIS_ERROR;
        return 229 + ( 10000 * error * -1 );
    }

    if ( ( error = git_annotated_commit_lookup( &their_heads[0], repo_, &their_oids[0] ) ) < 0 ) {
        MessagesI.AppendMessageT( tr( "FETCH_HEAD contained improper OID, cannot merge" ) );
        analysisResult_ = ANALYSIS_ERROR;
        return 233 + ( 10000 * error * -1 );
    }

    git_merge_analysis_t manalysis;
    git_merge_preference_t mpreference;

    if ( ( error = git_merge_analysis( &manalysis, &mpreference, repo_, (const git_annotated_commit **) their_heads, 1 ) ) < 0 ) {
        MessagesI.AppendMessageT( tr( "Could not analyze merge, error code: %1" ).arg( error * -1 ) );
        analysisResult_ = ANALYSIS_ERROR;
        return 239 + ( 10000 * error * -1 );
    }

    unsigned int result = ANALYSIS_EMPTY;
    if ( manalysis == GIT_MERGE_ANALYSIS_NONE ) {
        result = ANALYSIS_NONE;
    }
    if ( manalysis & GIT_MERGE_ANALYSIS_NORMAL ) {
        result = result | ANALYSIS_NORMAL;
    }
    if ( manalysis & GIT_MERGE_ANALYSIS_UP_TO_DATE ) {
        result = result | ANALYSIS_UP_TO_DATE;
    }
    if ( manalysis & GIT_MERGE_ANALYSIS_FASTFORWARD ) {
        result = result | ANALYSIS_FASTFORWARD;
    }
    if ( manalysis & GIT_MERGE_ANALYSIS_UNBORN ) {
        result = result | ANALYSIS_UNBORN;
    }

    analysisResult_ = static_cast< AnalysisResult > ( result );

    retval += closeRepo();
    return retval;
}

int lgit::fastForwardSha( const std::string & target_branch, const std::string &tip_sha, CheckoutType type )
{
    int error, retval = 0;

    error = openRepo();
    if ( error > 0 ) {
        retval += error;
        MessagesI.AppendMessageT( "Could not open repository " + repo_path_ + " (9)" );
        return retval + 1000000 * 43;
    }

    git_oid oid;
    git_commit *new_tip_commit;

    if ( ( error = git_oid_fromstr( &oid, tip_sha.c_str() ) ) < 0 ) {
        MessagesI.AppendMessageT( tr( "FETCH_HEAD contained improper SHA, cannot fast-forward" ) );
        retval += closeRepo();
        return retval + 251 + ( 10000 * error * -1 );
    }

    // Find new tip commit, to be used as tree pointer in checkout
    if ( ( error = git_commit_lookup( &new_tip_commit, repo_, &oid ) ) < 0 ) {
        MessagesI.AppendMessageT( tr( "FETCH_HEAD contained improper OID, cannot fast-forward" ) );
        retval += closeRepo();
        return retval + 257 + ( 10000 * error * -1 );
    }

    git_checkout_options checkout_options;

    if ( ( error = git_checkout_init_options( &checkout_options, GIT_CHECKOUT_OPTIONS_VERSION ) ) < 0 ) {
        git_commit_free( new_tip_commit );
        MessagesI.AppendMessageT( tr( "Could not initialize checkout, cannot fast-forward" ) );
        analysisResult_ = ANALYSIS_ERROR;
        retval += closeRepo();
        return retval + 263 + ( 10000 * error * -1 );
    }

    checkout_options.notify_flags = GIT_CHECKOUT_NOTIFY_CONFLICT | GIT_CHECKOUT_NOTIFY_DIRTY |
                                    GIT_CHECKOUT_NOTIFY_UPDATED | GIT_CHECKOUT_NOTIFY_UNTRACKED |
                                    GIT_CHECKOUT_NOTIFY_IGNORED;
    checkout_options.notify_payload = static_cast< void * >( op_tracker_ );
    checkout_options.notify_cb = checkout_notify_cb;

    switch( type ) {
    case CHECKOUT_DRY_RUN:
        checkout_options.checkout_strategy = GIT_CHECKOUT_NONE;
        break;
    case CHECKOUT_MERGE:
        checkout_options.checkout_strategy = GIT_CHECKOUT_SAFE | GIT_CHECKOUT_ALLOW_CONFLICTS | GIT_CHECKOUT_CONFLICT_STYLE_MERGE;
        break;
    case CHECKOUT_FORCE:
        checkout_options.checkout_strategy = GIT_CHECKOUT_FORCE;
        break;
    default:
        MessagesI.AppendMessageT( "Internal error on checkout, could not determine checkout type" );
        retval += closeRepo();
        return retval + 269 + ( 10000 * error * -1 );
    }

    if ( ( error = git_checkout_tree( repo_, (git_object *) new_tip_commit, &checkout_options ) ) < 0 ) {
        git_commit_free( new_tip_commit );
        const char *spec_error = giterr_last()->message;
        MessagesI.AppendMessageT( "Checkout failed with git error: " + QString::fromUtf8( spec_error ) + QString( " (%1)" ).arg( error*-1 ) );
        retval += closeRepo();
        return retval + 271 + ( 10000 * error * -1 );
    }

    // Commit is never used from now on
    git_commit_free( new_tip_commit );

    if( type == CHECKOUT_DRY_RUN ) {
        return retval;
    }

    git_reference *branch_reference, *new_branch_reference;
    QString branch_spec = QString( "refs/heads/%1" ).arg( QString::fromStdString( target_branch ) );
    if ( ( error = git_reference_lookup( &branch_reference, repo_, branch_spec.toUtf8().constData() ) ) < 0 ) {
        const char *spec_error = giterr_last()->message;
        MessagesI.AppendMessageT( "Fast-forward failed with git error: " + QString::fromUtf8( spec_error ) + QString( " (%1)" ).arg( error*-1 ) +
                                  "Working tree has the new expected state, however libgit2 didn't update meta-data (i.e. the branch reference)" );
        retval += closeRepo();
        return retval + 277 + ( 10000 * error * -1 );
    }

    if ( ( error = git_reference_set_target( &new_branch_reference, branch_reference, &oid, "merge: Fast-forward" ) ) < 0 ) {
        git_reference_free( branch_reference );
        const char *spec_error = giterr_last()->message;
        MessagesI.AppendMessageT( "Fast-forward failed with git error (2): " + QString::fromUtf8( spec_error ) + QString( " (%1)" ).arg( error*-1 ) +
                                  "Working tree has the new expected state, however libgit2 didn't update meta-data (i.e. the branch reference)" );
        retval += closeRepo();
        return retval + 281 + ( 10000 * error * -1 );
    }

    git_reference_free( branch_reference );
    git_reference_free( new_branch_reference );
    retval += closeRepo();
    return retval;
}

int lgit::mergeBranch( const std::string & branch, const std::string & our_tip, const mybranch & their_fh_entry )
{
    int error, retval = 0;

    error = openRepo();
    if ( error > 0 ) {
        retval += error;
        MessagesI.AppendMessageT( "Could not open repository " + repo_path_ + " (9)" );
        return retval + 1000000 * 47;
    }

    git_merge_options merge_options;
    git_checkout_options checkout_options;

    if ( ( error = git_merge_init_options( &merge_options, GIT_CHECKOUT_OPTIONS_VERSION ) ) < 0 ) {
        MessagesI.AppendMessageT( tr( "Could not initialize merge" ) );
        retval += closeRepo();
        return retval + 331 + ( 10000 * error * -1 );
    }

    if ( ( error = git_checkout_init_options( &checkout_options, GIT_CHECKOUT_OPTIONS_VERSION ) ) < 0 ) {
        MessagesI.AppendMessageT( tr( "Could not initialize checkout, cannot merge" ) );
        retval += closeRepo();
        return retval + 337 + ( 10000 * error * -1 );
    }

    checkout_options.checkout_strategy = GIT_CHECKOUT_FORCE;
    checkout_options.notify_flags = GIT_CHECKOUT_NOTIFY_CONFLICT | GIT_CHECKOUT_NOTIFY_DIRTY |
                                    GIT_CHECKOUT_NOTIFY_UPDATED | GIT_CHECKOUT_NOTIFY_UNTRACKED |
                                    GIT_CHECKOUT_NOTIFY_IGNORED;
    checkout_options.notify_payload = static_cast< void * >( op_tracker_ );
    checkout_options.notify_cb = checkout_notify_cb;

    git_oid our_oid, their_oid;
    git_commit *our_tip_commit, *their_tip_commit;
    git_annotated_commit *their_tip_commit_fh;

    if ( ( error = git_oid_fromstr( &our_oid, our_tip.c_str() ) ) < 0 ) {
        MessagesI.AppendMessageT( tr( "Improper SHA tip of current branch" ) );
        retval += closeRepo();
        return retval + 283 + ( 10000 * error * -1 );
    }

    if ( ( error = git_oid_fromstr( &their_oid, their_fh_entry.tip_sha.c_str() ) ) < 0 ) {
        MessagesI.AppendMessageT( tr( "Improper SHA tip of fetched branch (i.e. of a remote branch)" ) );
        retval += closeRepo();
        return retval + 293 + ( 10000 * error * -1 );
    }

    if ( ( error = git_commit_lookup( &our_tip_commit, repo_, &our_oid ) ) < 0 ) {
        MessagesI.AppendMessageT( tr( "Cannot find current HEAD commit" ) );
        retval += closeRepo();
        return retval + 307 + ( 10000 * error * -1 );
    }

    if ( ( error = git_commit_lookup( &their_tip_commit, repo_, &their_oid ) ) < 0 ) {
        MessagesI.AppendMessageT( tr( "Cannot find tip commit of fetched branch (i.e. of a remote branch)" ) );
        git_commit_free( our_tip_commit );
        retval += closeRepo();
        return retval + 311 + ( 10000 * error * -1 );
    }

    if ( ( error = git_annotated_commit_from_fetchhead( &their_tip_commit_fh, repo_,
                                                        their_fh_entry.name.c_str(), their_fh_entry.fetch_head_remote_url.c_str(),
                                                        &their_oid ) ) < 0 ) {
        MessagesI.AppendMessageT( tr( "Cannot prepare for merge from FETCH_HEAD" ) );
        git_commit_free( our_tip_commit );
        git_commit_free( their_tip_commit );
        retval += closeRepo();
        return retval + 313 + ( 10000 * error * -1 );
    }

    if ( ( error = git_merge( repo_, const_cast< const git_annotated_commit ** > ( &their_tip_commit_fh ), 1, &merge_options, &checkout_options) ) < 0 ) {
        const char *spec_error = giterr_last()->message;
        MessagesI.AppendMessageT( tr( "Merge failed: %1" ).arg( QString::fromUtf8( spec_error ) ) );
        git_commit_free( our_tip_commit );
        git_commit_free( their_tip_commit );
        git_annotated_commit_free( their_tip_commit_fh );
        retval += closeRepo();
        return retval + 317 + ( 10000 * error * -1 );
    }

    git_commit_free( our_tip_commit );
    git_commit_free( their_tip_commit );
    git_annotated_commit_free( their_tip_commit_fh );
    return retval;
}

int lgit::indexHasConflicts( bool *result )
{
    int error, retval = 0;

    error = openRepo();
    if ( error > 0 ) {
        retval += error;
        MessagesI.AppendMessageT( "Could not open repository " + repo_path_ + " (9)" );
        return retval + 1000000 * 53;
    }

    git_index *index;
    if ( ( error = git_repository_index( &index, repo_ ) ) < 0 ) {
        MessagesI.AppendMessageT( "Cannot commit: could not open repository's index (2)" );
        retval += 353 + ( 10000 * error * -1 );
        retval += closeRepo();
        return retval;
    }

    if ( ( error = git_index_read( index, 1 ) ) < 0 ) {
        MessagesI.AppendMessageT( "Cannot commit: could not read already staged files (2)" );
        retval += 359 + ( 10000 * error * -1 );
        git_index_free( index );
        retval += closeRepo();
        return retval;
    }

    *result = git_index_has_conflicts( index ) == 1;

    git_index_free( index );
    retval += closeRepo();
    return retval;
}

int lgit::loadBranches(int type)
{
    int error, retval = 0;

    error = openRepo();
    if ( error > 0 ) {
        retval += error;
        MessagesI.AppendMessageT( "Could not open repository " + repo_path_ + " (5)" );
        return retval + 1000000 * 59;
    }

    git_branches_.list( repo_, type );

    retval += closeRepo();
    return retval;
}

int lgit::establishCurrent()
{
    int error, retval = 0;

    error = openRepo();
    if ( error > 0 ) {
        retval += error;
        MessagesI.AppendMessageT( "Could not open repository " + repo_path_ + " (6)" );
        return retval + 1000000 * 41;
    }

    git_current_.discover( repo_ );

    retval += closeRepo();
    return retval;
}

int lgit::listRemotes()
{
    int error, retval = 0;

    error = openRepo();
    if ( error > 0 ) {
        retval += error;
        MessagesI.AppendMessageT( "Could not open repository " + repo_path_ + " (7)" );
        return retval + 1000000 * 71;
    }

    git_remotes_.list( repo_ );

    retval += closeRepo();
    return retval;
}

int lgit::readLog( const QString & tip_sha, const QString & hide, bool equip )
{
    int error, retval = 0;

    error = openRepo();
    if ( error > 0 ) {
        retval += error;
        MessagesI.AppendMessageT( "Could not open repository " + repo_path_ + " (8)" );
        return retval + 1000000 * 73;
    }

    git_log_.clear();
    git_log_.log_of_tip( repo_, tip_sha.toStdString(), hide.toStdString() );

    if( equip ) {
        git_log_.equip_with_diff_data( repo_ );
    }

    retval += closeRepo();
    return retval;
}

std::vector<std::string> lgit::gatherCheckoutOpDataForType( CheckoutOperationEvent type )
{
    std::vector< std::string > accum;
    for ( std::vector< checkout_operation_event >::const_iterator it = checkout_operation_data_.begin();
          it != checkout_operation_data_.end() ; ++ it )
    {
        if( it->type == type ) {
            accum.push_back( it->path );
        }
    }

    return accum;
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

static int fetch_transfer_progress_cb( const git_transfer_progress *stats, void *payload )
{
    double progress = 0.0;

    if ( stats->received_objects == stats->total_objects ) {
        if( stats->total_deltas == 0 ) {
            progress = 0.5;
        } else {
            progress = 0.5 + 0.5 * stats->indexed_deltas / stats->total_deltas;
        }
    }
    else if ( stats->total_objects > 0 ) {
        progress = 0.5 * stats->received_objects / stats->total_objects;
    }

    if ( payload ) {
        GitOperationTracker *tracker = static_cast< GitOperationTracker * >( payload );
        if ( tracker ) {
            tracker->updateFetchProgress( progress );
        }
    }

    return 0;
}

static int checkout_notify_cb( git_checkout_notify_t why, const char *path,
                               const git_diff_file *baseline, const git_diff_file *target, const git_diff_file *workdir,
                               void *payload )
{
    GitOperationTracker *op_tracker = static_cast< GitOperationTracker * > ( payload );
    if( op_tracker ) {
        op_tracker->checkoutNotify( why, path );
    }

    return 0;
}
