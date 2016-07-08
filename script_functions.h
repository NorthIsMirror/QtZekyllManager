#ifndef SCRIPT_FUNCTIONS_H
#define SCRIPT_FUNCTIONS_H

#include <tuple>
#include <QString>

std::tuple< QString, QString, int > getRepoFromPath( const QString & path );
QString decode_zkiresize_exit_code(int exitCode);

#endif // SCRIPT_FUNCTIONS_H
