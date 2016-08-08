#ifndef LGIT_REMOTES_H
#define LGIT_REMOTES_H

#include <vector>
#include <string>

#include "git2.h"

struct remote {
    std::string name;
    std::string url;
    std::string push_url;
};

class lgit_remotes
{
public:
    lgit_remotes();

    void clear() { remotes_.clear(); }
    void list( git_repository * repo );
    const std::vector< remote > & raw_remotes() const { return remotes_; }

private:
    std::vector< remote > remotes_;
};

#endif // LGIT_REMOTES_H
