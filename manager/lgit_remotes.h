#ifndef LGIT_REMOTES_H
#define LGIT_REMOTES_H

#include <vector>
#include <string>

#include <QString>

#include "git2.h"

struct myremote {
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
    const std::vector< myremote > & raw_remotes() const { return remotes_; }

    const myremote & entry( int idx ) const { return remotes_[idx]; }
    int count() const { return remotes_.size(); }
    const myremote & findRemoteByUrl( const std::string & url ) const;
    const myremote & findRemoteByUrl( const QString & url ) const;

private:
    std::vector< myremote > remotes_;
    myremote dummy_entry_;
};

#endif // LGIT_REMOTES_H
