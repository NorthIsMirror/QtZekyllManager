#ifndef CODING_FUNCTIONS_H
#define CODING_FUNCTIONS_H
#include <tuple>
#include <vector>
#include <string>
#include <QString>
#include <QMap>

std::tuple< std::vector<std::string> , int > setIndex(int index);
void create_codes_map();
void create_rcodes_map();
void create_sites_maps();
std::tuple< int, QMap<QString,QString>, int > process_meta_data( const std::vector<int> & _bits );

#endif // CODING_FUNCTIONS_H
