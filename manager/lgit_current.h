#ifndef LGIT_CURRENT_H
#define LGIT_CURRENT_H

#include <string>

#include "git2.h"

enum { CURRENT_OID=1, CURRENT_SYM=2, CURRENT_TAG=4, CURRENT_INVALID_MAIN=8, CURRENT_INVALID_OID=16,
       CURRENT_INVALID_OID2=32, CURRENT_INVALID_SYM=64, CURRENT_INVALID_OTHER=128,
       CURRENT_INVALID_EMPTY=256 };

class lgit_current
{
public:
    lgit_current();

    int discover( git_repository *repo );

    void clear() { current_oid_.clear(); current_branch_.clear(); current_tag_.clear(); state_ = CURRENT_INVALID_EMPTY; }
    std::string & oid() { return current_oid_; }
    std::string & branch() { return current_branch_; }
    std::string & tag() { return current_tag_; }
    int state() { return state_; }

private:
    git_repository *cur_repo_;

    std::string current_oid_;
    std::string current_branch_;
    std::string current_tag_;
    int state_;
};

#endif // LGIT_CURRENT_H
