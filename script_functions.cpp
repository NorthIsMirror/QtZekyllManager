#include "script_functions.h"
#include <QStringList>
#include <QRegExp>
#include <QFileInfo>
#include <QDebug>

using namespace std;
std::tuple< QString, int > getRepoFromPath( const QString & path ) {
    QFileInfo fileInfo = QFileInfo(path);
    QStringList nodes = path.split("/");
    QString last = nodes.takeLast();
    qDebug() << nodes;
    qDebug() << "Last: " << last;

    if( !fileInfo.isDir() ) {
        qDebug() << "Path 1";
        nodes.removeLast();
        last = nodes.takeLast();
    }
    qDebug() << "Path 2";
    if( !fileInfo.isDir() ) {
        qDebug() << "Path 3";
        return make_tuple( path, 1 );
    }

    QRegExp rx("^([a-z0-9][a-z0-9])---([a-zA-Z0-9][a-zA-Z0-9-]*)---([a-zA-Z0-9_-]+)---([a-zA-Z0-9_-/.~]+)$");
    rx.setCaseSensitivity(Qt::CaseSensitive);
    QStringList result;
    if (rx.indexIn( last ) != -1) {
        qDebug() << "Path 4";
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
        return make_tuple( result.join(""), 0 );
    } else {
        qDebug() << "Path 5";
        return make_tuple( path, 2 );
    }
}
