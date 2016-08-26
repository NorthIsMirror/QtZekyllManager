#ifndef LGIT_STATUS_H
#define LGIT_STATUS_H

#include <utility>
#include <vector>
#include <string>

#include "git2.h"

class lgit_status
{
public:
    lgit_status();

    int status( git_repository *repo );
    const std::string & status_summary() const;
    bool has_unstaged_changes();
    bool has_staged_changes();
    bool has_untracked_files();
    void clear();

private:
    mutable bool is_text_up_to_date;
    mutable std::string status_text_;

    std::vector< std::string > submodules_names;
    std::vector< std::string > ignores;
    std::vector< std::string > conflicts;
    std::vector< std::string > modifications_idx_;
    std::vector< std::string > modifications_wd_;
    std::vector< std::string > additions_idx_;
    std::vector< std::string > additions_wd_;
    std::vector< std::string > deletions_idx_;
    std::vector< std::string > deletions_wd_;
    std::vector< std::string > type_changes_idx_;
    std::vector< std::string > type_changes_wd_;
    std::vector< std::pair< std::string, std::string > > renames_idx_;
    std::vector< std::pair< std::string, std::string > > renames_wd_;
};

#endif // LGIT_STATUS_H
