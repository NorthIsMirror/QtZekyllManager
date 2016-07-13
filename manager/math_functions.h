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



#endif // MATH_FUNCTIONS_H
