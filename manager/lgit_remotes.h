#ifndef LGIT_REMOTES_H
#define LGIT_REMOTES_H

#include <vector>
#include <string>

#include <QString>

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

    const remote & entry( int idx ) const { return remotes_[idx]; }
    int count() const { return remotes_.size(); }
    const remote & findRemoteByUrl( const std::string & url ) const;
    const remote & findRemoteByUrl( const QString & url ) const;

private:
    std::vector< remote > remotes_;
    remote dummy_entry_;
};

#endif // LGIT_REMOTES_H
