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

#ifndef SELECTEDZEKYLLS_H
#define SELECTEDZEKYLLS_H

#include <vector>
#include <map>
#include <string>
#include <utility>

// Holds which zekylls are selected, together with IDs
typedef std::vector<int> IDsVec;
typedef std::pair< IDsVec, bool > IDSelection;
typedef std::map< std::string, IDSelection > SelectedZekylls;

// More basic data structure that holds only IDs of zekyll
typedef std::map< std::string, IDsVec > ZekyllIDs;

// A structure used in final stage of establishing selectors
typedef std::map< std::string, bool > SelectorsMap;

#endif // SELECTEDZEKYLLS_H
