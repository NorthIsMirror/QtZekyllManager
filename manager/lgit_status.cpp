#include "lgit_status.h"

#include "singleton.h"
#include "messages.h"

#include <cstddef>
#include <sstream>

#include <QString>
#include <QDebug>

#include "git2/status.h"
#include "git2/submodule.h"
#include "git2/errors.h"

#define MessagesI Singleton<Messages>::instance()

static int submodule_callback( git_submodule *sm, const char *name, void *payload );

static QDebug operator << ( QDebug out, const std::string & str )
{
    out << QString::fromStdString(str);
    return out;
}

lgit_status::lgit_status() : is_text_up_to_date( false )
{

}

int lgit_status::status( git_repository *repo )
{
    int error, retval = 0;
    git_status_options opts;

    if ( ( error = git_status_init_options( &opts, GIT_STATUS_OPTIONS_VERSION ) ) < 0 ) {
        MessagesI.AppendMessageT( QString( "Could not initialize git status (exit code: %1)" ).arg( error ) );
        return 479 + ( error * 10000 * -1 );
    }

    opts.show = GIT_STATUS_SHOW_INDEX_AND_WORKDIR;
    opts.flags = GIT_STATUS_OPT_RENAMES_HEAD_TO_INDEX | GIT_STATUS_OPT_SORT_CASE_SENSITIVELY |
                GIT_STATUS_OPT_INCLUDE_UNTRACKED;

    git_status_list *slist;

    if ( ( error = git_status_list_new( &slist, repo, &opts ) ) < 0 ) {
        const char *spec_error = giterr_last()->message;
        MessagesI.AppendMessageT( QString( "Git status failed: %1 (%2)" ).arg( spec_error ).arg( error ) );
        return 487 + ( error * 10000 * -1 );
    }

    is_text_up_to_date = false;

    size_t ecount = git_status_list_entrycount( slist );
    const git_status_entry *entry;
    for ( size_t i = 0; i < ecount; ++ i ) {
        entry = git_status_byindex( slist, i );
        if( !entry ) {
            const char *spec_error = giterr_last()->message;
            MessagesI.AppendMessageT( QString( "Could not read status entry: %1" ).arg( spec_error ) );
            continue;
        }

        if ( entry->status == GIT_STATUS_CURRENT ) {
            continue;
        }

        // Index

        if ( entry->head_to_index ) {
            const char *old_path = entry->head_to_index->old_file.path;
            const char *new_path = entry->head_to_index->new_file.path;

            if ( entry->status & GIT_STATUS_INDEX_NEW ) {
                additions_idx_.push_back( std::string( new_path ? new_path : old_path ) );
            }

            if ( entry->status & GIT_STATUS_INDEX_MODIFIED ) {
                modifications_idx_.push_back( std::string( new_path ? new_path : old_path ) );
            }

            if ( entry->status & GIT_STATUS_INDEX_DELETED ) {
                deletions_idx_.push_back( std::string( new_path ? new_path : old_path ) );
            }

            if ( entry->status & GIT_STATUS_INDEX_TYPECHANGE ) {
                type_changes_idx_.push_back( std::string( new_path ? new_path : old_path ) );
            }

            if ( entry->status & GIT_STATUS_INDEX_RENAMED ) {
                std::pair< std::string, std::string > rename;
                rename.first = std::string( old_path );
                rename.second = std::string( new_path );
                renames_idx_.push_back( rename );
            }
        }

        // Workdir

        if( entry->index_to_workdir ) {
            const char *old_path = entry->index_to_workdir->old_file.path;
            const char *new_path = entry->index_to_workdir->new_file.path;

            if ( entry->status & GIT_STATUS_WT_NEW ) {
                additions_wd_.push_back( std::string( new_path ? new_path : old_path ) );
            }

            if ( entry->status & GIT_STATUS_WT_MODIFIED ) {
                modifications_wd_.push_back( std::string( new_path ? new_path : old_path ) );
            }

            if ( entry->status & GIT_STATUS_WT_DELETED ) {
                deletions_wd_.push_back( std::string( new_path ? new_path : old_path ) );
            }

            if ( entry->status & GIT_STATUS_WT_TYPECHANGE ) {
                type_changes_wd_.push_back( std::string( new_path ? new_path : old_path ) );
            }

            if ( entry->status & GIT_STATUS_WT_RENAMED ) {
                std::pair< std::string, std::string > rename;
                rename.first = std::string( old_path );
                rename.second = std::string( new_path );
                renames_wd_.push_back( rename );
            }

            // General
            if ( entry->status & GIT_STATUS_IGNORED ) {
                ignores.push_back( std::string( new_path ? new_path : old_path ) );
            }

            if ( entry->status & GIT_STATUS_CONFLICTED ) {
                conflicts.push_back( std::string( new_path ? new_path : old_path ) );
            }
        }
    }

    git_submodule_foreach( repo, submodule_callback, &submodules_names );

    git_status_list_free( slist );
    return retval;
}

const std::string & lgit_status::status_summary() const
{
    std::stringstream output;

    if( is_text_up_to_date ) {
        return status_text_;
    }

    output << "<b>Staged changes:</b>" << "<br/>" << std::endl;

    for ( std::vector< std::string > :: const_iterator it = additions_idx_.begin(); it != additions_idx_.end(); ++ it ) {
        output << "new file: " << *it << "<br/>" << std::endl;
    }

    for ( std::vector< std::string > :: const_iterator it = modifications_idx_.begin(); it != modifications_idx_.end(); ++ it ) {
        output << "modified: " << *it << "<br/>" << std::endl;
    }

    for ( std::vector< std::string > :: const_iterator it = deletions_idx_.begin(); it != deletions_idx_.end(); ++ it ) {
        output << "deleted: " << *it << "<br/>" << std::endl;
    }

    for ( std::vector< std::string > :: const_iterator it = type_changes_idx_.begin(); it != type_changes_idx_.end(); ++ it ) {
        output << "type change: " << *it << "<br/>" << std::endl;
    }

    for ( std::vector< std::pair< std::string, std::string > > :: const_iterator it = renames_idx_.begin(); it != renames_idx_.end(); ++ it ) {
        output << it->first << " ➡ " << it->second << "<br/>" << std::endl;
    }

    output << "<br/>" << std::endl << "<b>Workdir changes:</b>" << "<br/>" << std::endl;

    for ( std::vector< std::string > :: const_iterator it = modifications_wd_.begin(); it != modifications_wd_.end(); ++ it ) {
        output << "modified: " << *it << "<br/>" << std::endl;
    }

    for ( std::vector< std::string > :: const_iterator it = deletions_wd_.begin(); it != deletions_wd_.end(); ++ it ) {
        output << "deleted: " << *it << "<br/>" << std::endl;
    }

    for ( std::vector< std::string > :: const_iterator it = type_changes_wd_.begin(); it != type_changes_wd_.end(); ++ it ) {
        output << "type change: " << *it << "<br/>" << std::endl;
    }

    for ( std::vector< std::pair< std::string, std::string > > :: const_iterator it = renames_wd_.begin(); it != renames_wd_.end(); ++ it ) {
        output << it->first << " ➡ " << it->second << "<br/>" << std::endl;
    }

    output << "<br/>" << std::endl << "<b>Untracked files:</b>" << "<br/>" << std::endl;

    for ( std::vector< std::string > :: const_iterator it = additions_wd_.begin(); it != additions_wd_.end(); ++ it ) {
        output << *it << "<br/>" << std::endl;
    }

    output << "<br/>" << std::endl << "<b>Ignored files:</b>" << "<br/>" << std::endl;

    for ( std::vector< std::string > :: const_iterator it = ignores.begin(); it != ignores.end(); ++ it ) {
        output << *it << "<br/>" << std::endl;
    }

    output << "<br/>" << std::endl << "<b>Conflicts:</b>" << "<br/>" << std::endl;

    for ( std::vector< std::string > :: const_iterator it = conflicts.begin(); it != conflicts.end(); ++ it ) {
        output << *it << "<br/>" << std::endl;
    }

    output << "<br/>" << std::endl << "<b>Submodules:</b>" << "<br/>" << std::endl;

    for ( std::vector< std::string > :: const_iterator it = submodules_names.begin(); it != submodules_names.end(); ++ it ) {
        output << *it << "<br/>" << std::endl;
    }

    status_text_ = output.str();
    is_text_up_to_date = true;

    return status_text_;
}

bool lgit_status::has_unstaged_changes()
{
    return ( modifications_wd_.size() != 0 ) | ( deletions_wd_.size() != 0 ) |
            ( type_changes_wd_.size() != 0 ) | ( renames_wd_.size() != 0 ) |
            ( conflicts.size() != 0 );
}

bool lgit_status::has_staged_changes()
{
    return ( modifications_idx_.size() != 0 ) | ( additions_idx_.size() != 0 ) |
            ( deletions_idx_.size() != 0 ) | ( type_changes_idx_.size() != 0 ) |
            ( renames_idx_.size() != 0 );
}

bool lgit_status::has_untracked_files()
{
    return ( additions_wd_.size() != 0 );
}

void lgit_status::clear()
{
    submodules_names.clear();
    ignores.clear();
    conflicts.clear();
    modifications_idx_.clear();
    modifications_wd_.clear();
    additions_idx_.clear();
    additions_wd_.clear();
    deletions_idx_.clear();
    deletions_wd_.clear();
    type_changes_idx_.clear();
    type_changes_wd_.clear();

    status_text_.clear();
    is_text_up_to_date = true;
}

static int submodule_callback( git_submodule *, const char *name, void *payload ) {
    std::vector< std::string > *submodules_names = static_cast< std::vector< std::string > * > ( payload );
    if( submodules_names && name ) {
        submodules_names->push_back( std::string( name ) );
    }

    return 0;
}
