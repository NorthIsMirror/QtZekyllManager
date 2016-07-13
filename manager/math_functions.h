#ifndef MATH_FUNCTIONS_H
#define MATH_FUNCTIONS_H
#include <vector>
#include <tuple>

std::tuple< std::vector<char>, int> convert_integer_to_base_36( int number );
std::tuple< std::vector<char>, int> numbers_to_letters( const std::vector<int> & digits );
std::tuple< std::vector<int>, int> letters_to_numbers( const std::vector<char> & letters );
std::tuple< std::vector<int>, int> letters_to_numbers( const std::string & letters );
std::tuple< std::vector<char>, int, int > div2( const std::vector<char> & letters );
std::tuple< std::string, int, int > div2( const std::string & letters );
std::tuple< std::vector<int>, int > decode_zcode( const std::string & code );
std::tuple<int, int> get_integer_from_base_36( const std::string & letters );
std::tuple< std::vector<int>, int > arr_01_to_24_bit_pack_numbers( const std::vector<int> & bits );
std::tuple< std::vector<int>, int > str_01_to_24_bit_pack_numbers( const std::string & sbits );


#endif // MATH_FUNCTIONS_H
