#ifndef GIT_TAGS_H
#define GIT_TAGS_H

#include <vector>
#include <string>

#include "git2.h"

enum { TAG_TARGET_COMMIT=1, TAG_TARGET_TREE=2, TAG_TARGET_TAG=3, TAG_TARGET_OTHER=4, TAG_TARGET_UNKNOWN=5 };

struct mytag {
    std::string name;
    std::string targetSha;
    int targetType;
    bool targetExists;

    bool has_annotation;
    std::string message;
    std::string user;
    std::string email;
    std::string sha;

    mytag() : targetType( TAG_TARGET_UNKNOWN ), targetExists( false ), has_annotation( false )
    { }
};

class lgit_tags
{
public:
    lgit_tags();

    int list( git_repository *repo );

    const std::vector< mytag > & raw_tags() const { return tags_; }

    void clear() { tags_.clear(); }

    int deleteTag( git_repository *repo, const std::string & name );

private:
    std::vector< mytag > tags_;
};

#endif // GIT_TAGS_H
