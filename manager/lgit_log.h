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

#ifndef LGIT_LOG_H
#define LGIT_LOG_H

#include <vector>
#include <string>

#include "git2.h"

struct myhunk {
    std::string header;
    std::vector< std::string > ulines;

    myhunk( const char *_header ) : header( _header ) {}
};

struct my_diff_file {
    std::string old_path;
    std::string new_path;
    std::vector< myhunk > hunks;

    my_diff_file( const char *_old_path, const char *_new_path ) : old_path( _old_path ), new_path( _new_path )
    {
        is_addition = is_deletion = is_modification = is_rename = false;
        is_copy = is_conflict = is_other = false;
    }

    bool is_addition;
    bool is_deletion;
    bool is_modification;
    bool is_rename;
    bool is_copy;
    bool is_conflict;
    bool is_other;
};

struct log_entry {
    std::string sha;
    std::string author;
    std::string message;

    bool has_diff_data;
    std::vector< my_diff_file > diff_files;

    std::string diff_data_summary_html();

    log_entry() : has_diff_data( false ) {}
};

struct diff_run_state {
    log_entry *entry;

    bool is_addition;
    bool is_deletion;
    bool is_modification;
    bool is_rename;
    bool is_copy;
    bool is_conflict;
    bool is_other;

    diff_run_state() : is_addition( false ), is_deletion( false ), is_modification( false ),
                        is_rename( false ), is_copy( false ), is_conflict( false ), is_other( false )
    { }

    void clear() {
        is_addition = is_deletion = is_modification = false;
        is_rename = is_copy = is_conflict = is_other = false;
    }
};

class lgit_log
{
public:
    lgit_log();

    int peel_revision( git_repository *repo, git_tree **out, const std::string & revision, git_otype target_type );

    int log_of_tip( git_repository *repo, const std::string & sha, const std::string & hide );

    int equip_with_diff_data_single( git_repository *repo, log_entry & first, const log_entry & second );

    int equip_with_diff_data_first( git_repository *repo, log_entry & first );

    int equip_with_diff_data( git_repository *repo );

    const std::vector< log_entry > & raw_log() const { return entries_; }

    void clear() { entries_.clear(); }

    const log_entry & entry( int idx ) const { return entries_[ idx ]; }

    unsigned int count() const { return entries_.size(); }

private:
    std::vector< log_entry > entries_;
};

#endif // LGIT_LOG_H
