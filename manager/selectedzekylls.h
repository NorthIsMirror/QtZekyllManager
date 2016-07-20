#ifndef SELECTEDZEKYLLS_H
#define SELECTEDZEKYLLS_H

#include <vector>
#include <map>
#include <string>
#include <utility>

// Holds which zekylls are selected, together with IDs
typedef std::vector<int> IdsVec;
typedef std::pair< IdsVec, bool > IdSelection;
typedef std::map< std::string, IdSelection > SelectedZekylls;

#endif // SELECTEDZEKYLLS_H
