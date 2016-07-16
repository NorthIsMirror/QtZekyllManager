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

    if( fileInfo.isFile() && fileInfo.exists() ) {
        last = nodes.takeLast();
        fileInfo = QFileInfo( nodes.join("/") );
    }

    if( !fileInfo.isDir() ) {
        return make_tuple( path, nodes.join("/") + "/" + last, 1 );
    }

    QRegExp rx("^([a-z0-9][a-z0-9])---([a-zA-Z0-9][a-zA-Z0-9-]*)---([a-zA-Z0-9_-]+)---([a-zA-Z0-9_/.~-]+)$");
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

QString decode_zkiresize_exit_code( int exitCode ) {
    QString error_decode = "";
    if ( exitCode == 1) {
        error_decode = "Improper options";
    } else if ( exitCode == 2) {
        error_decode = "Negative index size";
    } else if ( exitCode == 3) {
        error_decode = "Maximum index size exceeded";
    } else if ( exitCode == 4) {
        error_decode = "Repository doesn't exist";
    } else if ( exitCode == 5) {
        error_decode = "Inconsistent index (1)";
    } else if ( exitCode == 6) {
        error_decode = "No size requested";
    } else if ( exitCode == 7) {
        error_decode = "No change in index size";
    } else if ( exitCode == 8) {
        error_decode = "No agreement to continue";
    } else if ( exitCode == 9) {
        error_decode = "Improper section given";
    } else if ( exitCode == 10) {
        error_decode = "Improper description given";
    } else if ( exitCode == 11) {
        error_decode = "Inconsistent index (2)";
    } else if ( exitCode == 12) {
        error_decode = "Inconsistent index (3)";
    } else if ( exitCode == 13) {
        error_decode = "No correct path or repo provided";
    } else if ( exitCode == 14) {
        error_decode = "Given index size (-s/--size) is not a number";
    } else if ( exitCode == 15) {
        error_decode = "Given index (-i/--index) is not a number";
    } else if ( exitCode == 16) {
        error_decode = "Index (-i/--index) cannot be 0";
    }
    return error_decode;
}

QString decode_zkrewrite_exit_code( int exitCode ) {
    QString error_decode;
    switch( exitCode )
    {

    case 40:
        error_decode = "<font color=red>Internal error: improper arguments given to zkrewrite</font>";
        break;
    case 49:
        error_decode = "<font color=red>Internal error: no correct path for zkrewrite</font>";
        break;
    case 41:
        error_decode = "<font color=red>zkrewrite reports the path to repo is incorrect</font>";
        break;
    case 42:
        error_decode = "<font color=red>zkrewrite reports duplicates in zekylls</font>";
        break;
    case 43:
        error_decode = "<font color=red>Internal error: zkrewrite reports entered string too long</font>";
        break;
    case 44:
        error_decode = "<font color=red>Internal error: zkrewrite reports entered string too short</font>";
        break;
    case 45:
        error_decode = "<font color=red>zkrewrite reports duplicates in zekylls</font>";
        break;
    case 46:
        error_decode = "<font color=red>zkrewrite reports there are no zekylls in the repo</font>";
        break;
    case 47:
        error_decode = "<font color=red>zkrewrite reports collisions in zekylls</font>";
        break;
    case 48:
        error_decode = "<font color=red>Internal error: zkrewrite reports error during processing</font>";
        break;
    case 128:
        error_decode = "<font color=red>Are all files added to git?</font>";
        break;
    default:
        error_decode = "Result of the operation";
        break;
    }

    return error_decode;
}

QString reverseQString( const QString & str)
{
    QString str_rev;
    for( QString::const_iterator i = str.constEnd(); i != str.constBegin(); )
    {
        -- i;
        str_rev.append( *i );
    }
    return str_rev;
}
