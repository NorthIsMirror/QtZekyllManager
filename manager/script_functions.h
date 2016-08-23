/*
 Copyright 2016 Sebastian Gniazdowski

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
QString decode_libgit2_error_code( int errorCode );
QString reverseQString( const QString & str);
int sectionOrderOnLZCSDE( LZCSDE & lzcsde );
bool to_int( const QString & str, int *ret, bool *ok = NULL );
bool to_int( const QString & str, int *ret, int *ret2, bool *_ok = NULL );
QString joinStdIntVector( const std::vector<int> & bits, const char *sep = NULL );
char * create_cstring( const char *in );

#endif // SCRIPT_FUNCTIONS_H
