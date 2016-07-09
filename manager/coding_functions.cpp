#include "coding_functions.h"
#include "math_functions.h"
#include <string>
#include <QDebug>

using namespace std;

// FUNCTION: setIndex {{{
// Sets ZKL_INDEX_ZEKYLLS array which contains all
// zekylls that potentially can be part of the index
//
std::tuple<vector<string>, int> setIndex(int index) {

    vector<string> ZKL_INDEX_ZEKYLLS;

    // Compute first element pointed to by index
    int first=(index-1)*150;

    int error=0;
    for ( int i=first; i<=(first+150-1); i ++ ) {
        // Convert the number to base 36 with leading zeros
        std::tuple< vector<char>, int > result = convert_integer_to_base_36(i);
        error += get<1>( result );
        string zekyll = string( get<0>(result).begin(), get<0>(result).end() );
        ZKL_INDEX_ZEKYLLS.push_back(zekyll);
    }
    // foreach(const string& s, ZKL_INDEX_ZEKYLLS) { qDebug() << QString(s.c_str()); }

    return make_tuple( ZKL_INDEX_ZEKYLLS, error );
}
// }}}

