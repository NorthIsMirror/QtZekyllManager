/*
 Copyright 2016 Sebastian Gniazdowski

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
    dummy_entry_.name = "-";
    dummy_entry_.url = "-";
    dummy_entry_.push_url = "-";
}

void lgit_remotes::list( git_repository *repo )
{
    int error;
    git_strarray remotes = {0, 0};

    if ( ( error = git_remote_list( &remotes, repo ) ) < 0 ) {
        MessagesI.AppendMessageT( QString( "Error during remote list (%1)" ).arg( error ) );
        return;
    }

    clear();

    for ( unsigned int i = 0; i < remotes.count; i ++ ) {
        myremote newremote;
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

const myremote & lgit_remotes::findRemoteByUrl( const std::string & url ) const
{
    for( std::vector< myremote >::const_iterator it = remotes_.begin(); it != remotes_.end(); it ++ ) {
        if( it->url == url ) {
            return *it;
        }
    }

    return dummy_entry_;
}

const myremote & lgit_remotes::findRemoteByUrl( const QString & url ) const
{
    return findRemoteByUrl( url.toStdString() );
}
