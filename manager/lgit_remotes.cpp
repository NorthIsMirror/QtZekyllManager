#include "lgit_remotes.h"

#include "singleton.h"
#include "messages.h"

#include <QDebug>

#include "git2/remote.h"

#define MessagesI Singleton<Messages>::instance()

static QDebug operator<<(QDebug out, const std::string & str)
{
    out << QString::fromStdString(str);
    return out;
}

lgit_remotes::lgit_remotes()
{

}

void lgit_remotes::list( git_repository *repo )
{
    int error;
    git_strarray remotes = {0, 0};

    if ( ( error = git_remote_list( &remotes, repo ) ) < 0 ) {
        MessagesI.AppendMessageT( QString( "Error during remote list (%1)" ).arg( error ) );
        return;
    }

    for ( unsigned int i = 0; i < remotes.count; i ++ ) {
        remote newremote;
        newremote.name = std::string( remotes.strings[i] );
        newremote.url = std::string();
        newremote.push_url = std::string();

        git_remote *gitremote;

        if ( ( error = git_remote_lookup( &gitremote, repo, newremote.name.c_str() ) ) < 0) {
            MessagesI.AppendMessageT( QString( "Error during remote lookup (%1)" ).arg( error ) );
            continue;
        }

        const char *url = git_remote_url( gitremote );
        if( url ) {
            newremote.url = std::string( url );
        }

        const char *pushurl = git_remote_pushurl( gitremote );
        if( pushurl ) {
            newremote.push_url = std::string( pushurl );
        }

        remotes_.push_back( newremote );

        git_remote_free( gitremote );
    }

    git_strarray_free( &remotes );
}
