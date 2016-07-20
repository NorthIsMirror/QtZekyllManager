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

#endif // SELECTEDZEKYLLS_H
