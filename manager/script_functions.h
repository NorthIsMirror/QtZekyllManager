#ifndef SCRIPT_FUNCTIONS_H
#define SCRIPT_FUNCTIONS_H

#include <tuple>
#include <QString>

enum {
    MY_ONLY_PATH = 1,
    MY_REPO_AND_PATH = 2,
    MY_GENERAL_ERROR = 3
};

std::tuple< QString, QString, int > getRepoFromPath( const QString & path );
QString decode_zkiresize_exit_code( int exitCode );
QString decode_zkrewrite_exit_code( int exitCode );
QString reverseQString( const QString & str);

#endif // SCRIPT_FUNCTIONS_H
