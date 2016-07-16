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
void create_helper_maps();

QMap< QString, QString > & getCodes();
QMap< QString, QString > & getRCodes();
QMap< QString, QString > & getSites();
QMap< QString, QString > & getRSites();
QMap< QString, QString > & getNames();

std::tuple< int, QMap<QString,QString>, int > process_meta_data( const std::vector<int> & _bits );
int BitsStart( std::vector<int> & dest );
int BitsWithPreamble( std::vector<int> & dest, const QString & type, const QString & data );
int BitsStop( std::vector<int> & dest );
std::tuple< bool, int > BitsCompareSuffix( const std::vector<int> & bits, const QString & strBits );
int BitsRemoveIfStartStop( std::vector<int> & bits );

#endif // CODING_FUNCTIONS_H
