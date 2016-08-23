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
