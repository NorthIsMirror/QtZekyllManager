#ifndef SCRIPT_FUNCTIONS_H
#define SCRIPT_FUNCTIONS_H

#include "lzcsde.h"
#include <tuple>
#include <QString>

enum REPO_PATH_SITUATION {
    MY_ONLY_PATH = 1,
    MY_REPO_AND_PATH = 2,
    MY_GENERAL_ERROR = 3
};

std::tuple< QString, QString, int > getRepoFromPath( const QString & path );
std::tuple< QString, QString, int > getPathFromRepo( const QString & base_path, const QString & repo_or_path );
QString decode_zkiresize_exit_code( int exitCode );
QString decode_zkrewrite_exit_code( int exitCode );
QString reverseQString( const QString & str);
int sectionOrderOnLZCSDE( LZCSDE & lzcsde );
bool to_int( const QString & str, int *ret, bool *ok = NULL );
bool to_int( const QString & str, int *ret, int *ret2, bool *_ok = NULL );

#endif // SCRIPT_FUNCTIONS_H
