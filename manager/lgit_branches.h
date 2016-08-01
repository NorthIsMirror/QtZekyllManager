#ifndef LGIT_BRANCHES_H
#define LGIT_BRANCHES_H

#include "git2.h"

#include <string>
#include <vector>

enum { BRANCH_UNKNOWN=1, BRANCH_LOCAL=2, BRANCH_REMOTE=3, BRANCH_ALL=4 };

enum { INVALID_CORRECT=1, INVALID_TYPE=2 /* fatal */, INVALID_TYPE2=4 /* fatal */, INVALID_TYPE3=8 /* fatal */,
                          INVALID_OID=16 /* fatal */, INVALID_OID2=32 /* fatal */,
                          INVALID_SYM=64 /* fatal */, INVALID_SHATWO=128 /* not fatal */,
                          INVALID_SHATWO2=256 /* not fatal */, INVALID_SHATWO3=512 /* not fatal */,
                          INVALID_BRANCH=1024 /* fatal */, INVALID_DUMMY=2048 /* fatal */
     };

struct mybranch {
    std::string tip_sha;
    std::vector< std::string > parents_shas;

    std::string name;

    int type;
    bool is_checked_out;

    bool is_in_fetch_head;
    bool is_pure_fetch_head;
    bool fetch_head_is_merge;
    std::string fetch_head_remote_url;

    bool symbolic;
    std::string symbolic_ref;

    int invalid;

    mybranch() : tip_sha( "" ), parents_shas( std::vector<std::string>() ), name( "" ), type( BRANCH_UNKNOWN ),
               is_checked_out( false ), is_in_fetch_head( false ), is_pure_fetch_head( false ), fetch_head_is_merge( false ),
               fetch_head_remote_url( "" ), symbolic( false ), symbolic_ref( "" ), invalid( INVALID_CORRECT )
    { }
};

class lgit_branches
{
public:
    lgit_branches();

    // Frees current branches list, gets new one
    int list( git_repository *repo, int type );
    // Fills .parent_shas in given mybranch
    int fill_oid_parents( mybranch & branch, const git_oid *oid );

    void set_cur_repo( git_repository *cur_repo ) { cur_repo_ = cur_repo; }
    git_repository *cur_repo() { return cur_repo_; }

    const std::vector< mybranch > & raw_branches() const { return raw_branches_; }
    void add( const mybranch & branch ) { raw_branches_.push_back( branch ); }
    mybranch & operator[] ( int idx ) { return raw_branches_[ idx ]; }
    const mybranch & operator[] ( int idx ) const { return raw_branches_[ idx ]; }
    int count() const { return raw_branches_.size(); }
    int size() const { return raw_branches_.size(); }

    mybranch & findSha( const char *sha );

private:
    std::vector< mybranch > raw_branches_;
    git_repository *cur_repo_;
};

#endif // LGIT_BRANCHES_H
