#include "math_functions.h"
#include <QDebug>
#include <algorithm>
#include "singleton.h"
#include "messages.h"

#define MessagesI Singleton<Messages>::instance()

const char characters[37]={ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
                      'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '\0' };

const char *characters_begin = &characters[0];
const char *characters_end = &characters[36];

// FUNCTION: convert_integer_to_base_36 {{{
// Takes number, returns string [a-z0-9]+ that
// is representation of the number in base 36

std::tuple< std::vector<char>, int > convert_integer_to_base_36(int number) {
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

    std::tuple<std::vector<char>, int> letters_error = numbers_to_letters(digits);

    return letters_error;
}
// }}}

// FUNCTION: numbers_to_letters {{{
// Converts array of numbers into string [a-z0-9]+
std::tuple< std::vector<char>, int> numbers_to_letters(const std::vector<int> & digits) {
    std::vector<char> REPLY;
    for( std::vector<int>::const_iterator it = digits.begin(); it != digits.end(); it ++ ) {
        int i = *it;
        if ( i < 0 || i > 35 ) {
            MessagesI.AppendMessageT( "Incorrect number during character conversion: " + QString("%1").arg(i) );
            return std::tuple<std::vector<char>, int>( REPLY, 1 );
        }
        REPLY.push_back( characters[i] );
    }

    return std::tuple<std::vector<char>, int>( REPLY, 0 );
}

// FUNCTION: letters_to_numbers {{{
// Converts each character of input string into number 0..35
std::tuple< std::vector<int>, int> letters_to_numbers( const std::vector<char> & letters ) {
    std::vector<int> reply;

    QString str = QString::fromLatin1( &letters[0], letters.size() );
    QRegExp rx("^[a-z0-9]+$");
    rx.setCaseSensitivity( Qt::CaseSensitive );
    if ( rx.indexIn( str ) == -1 ) {
        MessagesI.AppendMessageT( "Incorrect character during conversion, allowed are a-z and 0-9" );
        return make_tuple( reply, 1 );
    }

    int number;
    for( std::vector<char>::const_iterator it = letters.begin(); it != letters.end(); it ++ ) {
        const char *number_it = std::find( characters_begin, characters_end, *it );
        number = std::distance( characters_begin, number_it );
        reply.push_back( number );
    }

    return make_tuple( reply, 0 );
}

std::tuple< std::vector<int>, int> letters_to_numbers( const std::string & letters ) {
    std::vector<char> letters2( letters.c_str(), letters.c_str() + letters.size() );
    return letters_to_numbers( letters2 );
}

// FUNCTION: div2 {{{
// input - zcode's letters
// Result – ( "zcode's letters after division" "remainder 0 or 1" )
std::tuple< std::vector<char>, int, int > div2( const std::vector<char> & letters ) {
    // First translate the letters to numbers and put them into array
    std::tuple< std::vector<int>, int > res = letters_to_numbers( letters );
    int error = std::get<1>( res );
    if( error != 0 ) {
        return make_tuple( std::vector<char>(), 0, error );
    }

    std::vector<int> numbers = std::get<0>( res );

    // Now operate on the array performing long-division
    int cur = 0, last = numbers.size() - 1;

    std::vector<int> result;

    int prepared_for_division = numbers[cur];
    int subtracted;
    while (( 1 )) {
        int quotient = prepared_for_division / 2;

        result.push_back( quotient );

        int recovered = quotient*2;
        subtracted = prepared_for_division - recovered;

        cur ++;
        if ( cur > last ) {
            break;
        }

        prepared_for_division = 36 * subtracted + numbers[cur];
    }

    // Now convert the result to letters

    std::tuple< std::vector<char>, int > res2 = numbers_to_letters( result );
    error = std::get<1>(res2);
    if( error != 0 ) {
        return make_tuple( std::vector<char>(), 0, error );
    }

    return make_tuple( std::get<0>( res2 ), subtracted, 0 );
}
// }}}

// FUNCTION: div2 {{{
std::tuple< std::string, int, int > div2( const std::string & letters ) {
    std::vector<char> out_letters;
    int subtracted, error;
    tie( out_letters, subtracted, error ) = div2( std::vector<char>( letters.c_str(), letters.c_str() + letters.size() ) );

    return make_tuple( std::string( out_letters.begin(), out_letters.end() ), subtracted, error );
}
// }}}
