#include "coding_functions.h"
#include "math_functions.h"
#include <string>
#include <QDebug>
#include <QMap>

using namespace std;


QMap<QString, QString> codes;
QMap<QString, QString> rcodes;

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



void create_codes_map() {
    codes["ss"]      = "101000";
    codes["file"]    = "100111";
    codes["rev"]     = "101110";
    codes["repo"]    = "101111";
    codes["wordrev"] = "101100";
    codes["chksum"]  = "101101";
    codes["site"]    = "100010";
    codes["unused1"] = "100011";
    codes["unused2"] = "100000";
    codes["unused3"] = "1100110";
    codes["b"] = "110001";
    codes["a"] = "110000";
    codes["9"] = "101011";
    codes["8"] = "101010";
    codes["."] = "101001";
    codes["/"] = "100110";
    codes["_"] = "100101";
    codes["-"] = "100100";
    codes["~"] = "100001";
    codes["x"] = "011111";
    codes["w"] = "011110";
    codes["z"] = "011101";
    codes["y"] = "011100";
    codes["t"] = "011011";
    codes["s"] = "011010";
    codes["v"] = "011001";
    codes["u"] = "011000";
    codes["5"] = "010111";
    codes["4"] = "010110";
    codes["7"] = "010101";
    codes["6"] = "010100";
    codes["1"] = "010011";
    codes["0"] = "010010";
    codes["3"] = "010001";
    codes["2"] = "010000";
    codes["h"] = "001111";
    codes["g"] = "001110";
    codes["j"] = "001101";
    codes["i"] = "001100";
    codes["d"] = "001011";
    codes["c"] = "001010";
    codes["f"] = "001001";
    codes["e"] = "001000";
    codes["p"] = "000111";
    codes["o"] = "000110";
    codes["r"] = "000101";
    codes["q"] = "000100";
    codes["l"] = "000011";
    codes["k"] = "000010";
    codes["n"] = "000001";
    codes["m"] = "000000";
    codes["J"] = "1111111";
    codes["I"] = "1111110";
    codes["L"] = "1111101";
    codes["K"] = "1111100";
    codes["F"] = "1111011";
    codes["E"] = "1111010";
    codes["H"] = "1111001";
    codes["G"] = "1111000";
    codes["R"] = "1110111";
    codes["Q"] = "1110110";
    codes["T"] = "1110101";
    codes["S"] = "1110100";
    codes["N"] = "1110011";
    codes["M"] = "1110010";
    codes["P"] = "1110001";
    codes["O"] = "1110000";
    codes["Z"] = "1101111";
    codes["Y"] = "1101110";
    codes[" "] = "1101101";
    codes["A"] = "1101100";
    codes["V"] = "1101011";
    codes["U"] = "1101010";
    codes["X"] = "1101001";
    codes["W"] = "1101000";
    codes["B"] = "1100111";
    codes["D"] = "1100101";
    codes["C"] = "1100100";
}

void create_rcodes_map() {
    rcodes["101000"] = "ss";
    rcodes["100111"] = "file";
    rcodes["101110"] = "rev";
    rcodes["101111"] = "repo";
    rcodes["101100"] = "wordrev";
    rcodes["101101"] = "chksum";
    rcodes["100010"] = "site";
    rcodes["100011"] = "unused1";
    rcodes["100000"] = "unused2";
    rcodes["1100110"] = "unused3";
    rcodes["110001"] = "b";
    rcodes["110000"] = "a";
    rcodes["101011"] = "9";
    rcodes["101010"] = "8";
    rcodes["101001"] = ".";
    rcodes["100110"] = "/";
    rcodes["100101"] = "_";
    rcodes["100100"] = "-";
    rcodes["100001"] = "~";
    rcodes["011111"] = "x";
    rcodes["011110"] = "w";
    rcodes["011101"] = "z";
    rcodes["011100"] = "y";
    rcodes["011011"] = "t";
    rcodes["011010"] = "s";
    rcodes["011001"] = "v";
    rcodes["011000"] = "u";
    rcodes["010111"] = "5";
    rcodes["010110"] = "4";
    rcodes["010101"] = "7";
    rcodes["010100"] = "6";
    rcodes["010011"] = "1";
    rcodes["010010"] = "0";
    rcodes["010001"] = "3";
    rcodes["010000"] = "2";
    rcodes["001111"] = "h";
    rcodes["001110"] = "g";
    rcodes["001101"] = "j";
    rcodes["001100"] = "i";
    rcodes["001011"] = "d";
    rcodes["001010"] = "c";
    rcodes["001001"] = "f";
    rcodes["001000"] = "e";
    rcodes["000111"] = "p";
    rcodes["000110"] = "o";
    rcodes["000101"] = "r";
    rcodes["000100"] = "q";
    rcodes["000011"] = "l";
    rcodes["000010"] = "k";
    rcodes["000001"] = "n";
    rcodes["000000"] = "m";
    rcodes["1111111"] = "J";
    rcodes["1111110"] = "I";
    rcodes["1111101"] = "L";
    rcodes["1111100"] = "K";
    rcodes["1111011"] = "F";
    rcodes["1111010"] = "E";
    rcodes["1111001"] = "H";
    rcodes["1111000"] = "G";
    rcodes["1110111"] = "R";
    rcodes["1110110"] = "Q";
    rcodes["1110101"] = "T";
    rcodes["1110100"] = "S";
    rcodes["1110011"] = "N";
    rcodes["1110010"] = "M";
    rcodes["1110001"] = "P";
    rcodes["1110000"] = "O";
    rcodes["1101111"] = "Z";
    rcodes["1101110"] = "Y";
    rcodes["1101101"] = " ";
    rcodes["1101100"] = "A";
    rcodes["1101011"] = "V";
    rcodes["1101010"] = "U";
    rcodes["1101001"] = "X";
    rcodes["1101000"] = "W";
    rcodes["1100111"] = "B";
    rcodes["1100101"] = "D";
    rcodes["1100100"] = "C";
}
