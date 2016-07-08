#include "math_functions.h"
#include <QDebug>
#include <algorithm>

char characters[36]={ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
                      'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z' };

// FUNCTION: convert_integer_to_base_36 {{{
// Takes number, returns string [a-z0-9]+ that
// is representation of the number in base 36

std::vector<char> convert_integer_to_base_36(int number) {
    std::vector<int> digits;

    int new_number = number;
    int remainder;
    while ( new_number != 0 ) {
        remainder = new_number % 36;
        new_number = new_number/36;

        digits.push_back( remainder );
    }

    if ( digits.size() == 0 )
        digits.push_back(0);
    if ( digits.size() == 1 )
        digits.push_back(0);
    if ( digits.size() == 2 )
        digits.push_back(0);

    std::reverse(digits.begin(), digits.end());

    std::vector<char> letters = numbers_to_letters(digits);

    return letters;
}
// }}}

// FUNCTION: numbers_to_letters {{{
// Converts array of numbers into string [a-z0-9]+
std::vector<char> numbers_to_letters(const std::vector<int> & digits) {
    std::vector<char> REPLY;
    for( std::vector<int>::const_iterator it = digits.begin(); it != digits.end(); it ++ ) {
        int i = *it;
        if ( i < 0 || i > 35 ) {
            qDebug() << "Incorrect number during character conversion: " << i;
            return REPLY;
        }
        REPLY.push_back( characters[i] );
    }

    return REPLY;
}
