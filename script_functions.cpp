#include "script_functions.h"
#include <QStringList>
#include <QRegExp>
#include <QFileInfo>
#include <QDebug>

using namespace std;
std::tuple< QString, QString, int > getRepoFromPath( const QString & path ) {
    QFileInfo fileInfo = QFileInfo(path);
    QStringList nodes = path.split("/");
    QString last = nodes.takeLast();

    if( !fileInfo.isDir() ) {
        last = nodes.takeLast();
        fileInfo = QFileInfo( nodes.join("/") );
    }

    if( !fileInfo.isDir() ) {
        return make_tuple( path, nodes.join("/") + "/" + last, 1 );
    }

    QRegExp rx("^([a-z0-9][a-z0-9])---([a-zA-Z0-9][a-zA-Z0-9-]*)---([a-zA-Z0-9_-]+)---([a-zA-Z0-9_-/.~]+)$");
    rx.setCaseSensitivity(Qt::CaseSensitive);
    QStringList result;
    if (rx.indexIn( last ) != -1) {
        if( rx.cap(1) != "gh" ) {
            result += rx.cap(1);
            result += "@";
        }
        result += rx.cap(2);
        result += "/";
        result += rx.cap(3);
        if( rx.cap(4) != "master" ) {
            result += "/";
            result += rx.cap(4);
        }
        return make_tuple( result.join(""), nodes.join("/") + "/" + last, 0 );
    } else {
        return make_tuple( path, nodes.join("/") + "/" + last, 2 );
    }
}
