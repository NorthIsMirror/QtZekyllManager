#ifndef LGIT_CURRENT_H
#define LGIT_CURRENT_H

#include <string>

#include "git2.h"

enum { CURRENT_OID=1, CURRENT_SYM=2, CURRENT_INVALID_MAIN=4, CURRENT_INVALID_OID=8,
       CURRENT_INVALID_OID2=16, CURRENT_INVALID_SYM=32, CURRENT_INVALID_OTHER=64,
       CURRENT_INVALID_EMPTY=128 };

enum { CURRENT_TYPE_UNSET=1, CURRENT_TYPE_OID=2, CURRENT_TYPE_BRANCH=4, CURRENT_TYPE_TAG=8 };

class lgit_current
{
public:
    lgit_current();

    int discover( git_repository *repo );

    void clear() { current_oid_.clear(); current_branch_.clear(); current_tag_.clear(); state_ = CURRENT_INVALID_EMPTY; type_ = CURRENT_TYPE_UNSET; }
    const std::string & oid() const { return current_oid_; }
    const std::string & branch() const { return current_branch_; }
    const std::string & tag() const { return current_tag_; }
    int state() const { return state_; }
    int type() const { return type_; }

private:
    git_repository *cur_repo_;

    std::string current_oid_;
    std::string current_branch_;
    std::string current_tag_;
    int state_;
    int type_;
};

#endif // LGIT_CURRENT_H
