#ifndef LGIT_LOG_H
#define LGIT_LOG_H

#include <vector>
#include <string>

#include "git2.h"

struct log_entry {
    std::string sha;
    std::string author;
    std::string message;
};

class lgit_log
{
public:
    lgit_log();

    int log_of_tip( git_repository *repo, const std::string & sha, const std::string & hide );

    const std::vector< log_entry > & raw_log() const { return entries_; }

    void clear() { entries_.clear(); }

    const log_entry & entry( int idx ) const { return entries_[ idx ]; }

    int count() const { return entries_.size(); }

private:
    std::vector< log_entry > entries_;
};

#endif // LGIT_LOG_H
