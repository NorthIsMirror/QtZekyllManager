#include "script_functions.h"

#include <sstream>
#include <algorithm>
#include <cstring>

#include <QStringList>
#include <QRegExp>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

// Own copy of enum from git2/errors.h
typedef enum {
        LGIT_OK         =  0,

        LGIT_ERROR      = -1,
        LGIT_ENOTFOUND  = -3,
        LGIT_EEXISTS    = -4,
        LGIT_EAMBIGUOUS = -5,
        LGIT_EBUFS      = -6,

        LGIT_EUSER      = -7,

        LGIT_EBAREREPO       =  -8,
        LGIT_EUNBORNBRANCH   =  -9,
        LGIT_EUNMERGED       = -10,
        LGIT_ENONFASTFORWARD = -11,
        LGIT_EINVALIDSPEC    = -12,
        LGIT_ECONFLICT       = -13,
        LGIT_ELOCKED         = -14,
        LGIT_EMODIFIED       = -15,
        LGIT_EAUTH           = -16,
        LGIT_ECERTIFICATE    = -17,
        LGIT_EAPPLIED        = -18,
        LGIT_EPEEL           = -19,
        LGIT_EEOF            = -20,
        LGIT_EINVALID        = -21,
        LGIT_EUNCOMMITTED    = -22,
        LGIT_EDIRECTORY      = -23,
        LGIT_EMERGECONFLICT  = -24,

        LGIT_PASSTHROUGH     = -30,
        LGIT_ITEROVER        = -31,
} lLGIT_error_code;

using namespace std;
std::tuple< QString, QString, int > getRepoFromPath( const QString & path ) {
    QFileInfo fileInfo = QFileInfo(path);
    QStringList nodes = path.split("/");
    QString last = nodes.takeLast();

    if( fileInfo.isFile() && fileInfo.exists() ) {
        // Remove last path node, as it is regular file
        last = nodes.takeLast();
        fileInfo = QFileInfo( nodes.join("/") );
    }

    if( !fileInfo.isDir() ) {
        // Couldn't find directory at given path,
        // return the path unmodified and our
        // possibly processed version
        return make_tuple( path, nodes.join("/") + "/" + last, MY_GENERAL_ERROR );
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
        return make_tuple( result.join(""), nodes.join("/") + "/" + last, MY_REPO_AND_PATH );
    } else {
        // In case of this partial error, second tuple's element contains the
        // returned path and first element – as a fallback – the input path
        return make_tuple( path, nodes.join("/") + "/" + last, MY_ONLY_PATH );
    }
}

std::tuple< QString, QString, int > getPathFromRepo( const QString & _base_path, const QString & _repo_or_path ) {
    QString base_path = _base_path.trimmed();
    QString repo_or_path = _repo_or_path.trimmed();

    // Both mark if a return data is available (if not empty)
    QString last_node, path;

    // First check if the string matches repo spec
    QRegExp rx;

    // xy@user/repo/rev
    rx = QRegExp( "^([a-zA-Z][a-zA-Z])@([a-zA-Z0-9][a-zA-Z0-9-]*)[/]([a-zA-Z0-9_-]+)[/]([a-zA-Z0-9_-]+)$" );
    if( rx.indexIn( repo_or_path ) != -1 ) {
        last_node = rx.cap(1) + "---" + rx.cap(2) + "---" + rx.cap(3) + "---" + rx.cap(4);
    } else {
        // user/repo/rev
        rx = QRegExp( "^([a-zA-Z0-9][a-zA-Z0-9-]*)[/]([a-zA-Z0-9_-]+)[/]([a-zA-Z0-9_-]+)$" );
        if( rx.indexIn( repo_or_path ) != -1 ) {
            last_node = QString("gh---") + rx.cap(1) + "---" + rx.cap(2) + "---" + rx.cap(3);
        } else {
            // xy@user/repo
            rx = QRegExp( "^([a-zA-Z][a-zA-Z])@([a-zA-Z0-9][a-zA-Z0-9-]*)[/]([a-zA-Z0-9_-]+)$" );
            if( rx.indexIn( repo_or_path ) != -1 ) {
                last_node = rx.cap(1) + "---" + rx.cap(2) + "---" + rx.cap(3) + "---master";
            } else {
                // user/repo
                rx = QRegExp( "^([a-zA-Z0-9][a-zA-Z0-9-]*)[/]([a-zA-Z0-9_-]+)$" );
                if( rx.indexIn( repo_or_path ) != -1 ) {
                    last_node = QString("gh---") + rx.cap(1) + "---" + rx.cap(2) + "---master";
                } else {
                    // user
                    rx = QRegExp( "^([a-zA-Z0-9][a-zA-Z0-9-]*)$" );
                    if( rx.indexIn( repo_or_path ) != -1 ) {
                        last_node = QString("gh---") + rx.cap(1) + "---zkl---master";
                    }
                }
            }
        }
    }

    if( last_node.size() > 0 ) {
        QFileInfo finfo = QFileInfo( QDir(_base_path), last_node ).absoluteFilePath();
        path = finfo.absoluteFilePath();
        if( finfo.exists() ) {
            return std::make_tuple( repo_or_path, path, MY_REPO_AND_PATH );
        } else {
            // Repeat input data in first element, return faulty path in second element
            return std::make_tuple( repo_or_path, path, MY_GENERAL_ERROR );
        }
    } else {
        QFileInfo finfo( repo_or_path );
        if( !finfo.isAbsolute() ) {
            finfo = QFileInfo( QDir( base_path ), repo_or_path );
        }

        if( !finfo.exists() ) {
            finfo = QFileInfo( finfo.dir(), "" );
        }

        // Shouldn't go further than base_path, _repo_or_path must provide something
        if( finfo.absoluteFilePath() == base_path ) {
            // Repeat input data in first element, return faulty path in second element
            return std::make_tuple( repo_or_path, finfo.absoluteFilePath(), MY_GENERAL_ERROR );
        }

        // Remember something for erroneous return
        path = finfo.absoluteFilePath();

        if( finfo.exists() ) {
            if( finfo.isFile() ) {
                finfo = QFileInfo( finfo.dir(), "" );
            }

            path = finfo.absoluteFilePath();

            // Just repeat the input data in first element, the actual
            // output is the second element
            return std::make_tuple( repo_or_path, path, MY_ONLY_PATH );
        }
    }

    // Repeat input data in first element, return "something" in second
    return std::make_tuple( repo_or_path, path, MY_GENERAL_ERROR );
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

QString decode_libgit2_error_code( int errorCode ) {
    QString error_decode;
    switch( errorCode )
    {
    case LGIT_OK:
        error_decode = "No actual error";
        break;
    case LGIT_ERROR:
        error_decode = "Generic error";
        break;
    case LGIT_ENOTFOUND:
        error_decode = "Requested object could not be found";
        break;
    case LGIT_EEXISTS:
        error_decode = "Object already exists preventing operation";
        break;
    case LGIT_EAMBIGUOUS:
        error_decode = "More than one object matches";
        break;
    case LGIT_EBUFS:
        error_decode = "Output buffer too short to hold data";
        break;
    case LGIT_EUSER:
        error_decode = "No actual error";
        break;
    case LGIT_EBAREREPO:
        error_decode = "Operation not allowed on bare repository";
        break;
    case LGIT_EUNBORNBRANCH:
        error_decode = "HEAD refers to branch with no commits";
        break;
    case LGIT_EUNMERGED:
        error_decode = "There is a merge in progress, this prevents operation";
        break;
    case LGIT_ENONFASTFORWARD:
        error_decode = "Reference is not fast-forwardable";
        break;
    case LGIT_EINVALIDSPEC:
        error_decode = "Name/ref_spec is not in a valid format";
        break;
    case LGIT_ECONFLICT:
        error_decode = "Checkout conflicts prevented operation";
        break;
    case LGIT_ELOCKED:
        error_decode = "Lock file prevented operation";
        break;
    case LGIT_EMODIFIED:
        error_decode = "Reference's value does not match expected";
        break;
    case LGIT_EAUTH:
        error_decode = "Authentication error";
        break;
    case LGIT_ECERTIFICATE:
        error_decode = "Server certificate is invalid";
        break;
    case LGIT_EAPPLIED:
        error_decode = "Patch/merge has already been applied";
        break;
    case LGIT_EPEEL:
        error_decode = "The requested peel operation is not possible";
        break;
    case LGIT_EEOF:
        error_decode = "Unexpected end of data";
        break;
    case LGIT_EINVALID:
        error_decode = "Invalid operation or input";
        break;
    case LGIT_EUNCOMMITTED:
        error_decode = "Uncommitted changes prevented operation";
        break;
    case LGIT_EDIRECTORY:
        error_decode = "The operation is not valid for a directory";
        break;
    case LGIT_EMERGECONFLICT:
        error_decode = "A merge conflict exists – cannot continue";
        break;
    case LGIT_PASSTHROUGH:
        error_decode = "Internal error";
        break;
    case LGIT_ITEROVER:
        error_decode = "Internal error (iterators) – not an actual error, but should not happen";
        break;
    default:
        error_decode = "Unknown Git error";
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

char section_letters[26] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z' };

int sectionOrderOnLZCSDE( LZCSDE & lzcsde ) {
    int error = 0;
    QVector<LZCSDE_Entry> new_entries;
    const QVector<LZCSDE_Entry> & entries = lzcsde.entries();
    int size = entries.count();

    for( int a = 0; a < 26; ++ a ) {
        char selected_section = section_letters[a];
        for( int i = 0; i < size; ++ i ) {
            if( QString( selected_section ) != entries[i].section() ) {
                continue;
            }

            new_entries.push_back( entries[i] );
        }
    }

    if( new_entries.count() != entries.count() ) {
        error = 1;
    } else {
        lzcsde.entries() = new_entries;
    }

    return error;
}

bool to_int( const QString & str, int *ret, bool *_ok ) {
    if( !ret ) {
        return false;
    }
    bool ok = false;
    *ret = str.toInt( &ok );
    if( _ok ) {
        *_ok = ok;
    }

    return ok;
}

bool to_int( const QString & str, int *ret, int *ret2, bool *_ok ) {
    if( to_int( str, ret, _ok ) ) {
        *ret2 = *ret;
        return true;
    }

    return false;
}

QString joinStdIntVector( const std::vector<int> & bits, const char *sep ) {
    if( !sep ) {
        sep = "";
    }
    std::stringstream ss;
    std::copy( bits.begin(), bits.end(), std::ostream_iterator<int>( ss, sep ) );
    return QString::fromStdString( ss.str() );
}

char * create_cstring( const char *in ) {
    int len = std::strlen( in );
    char *out = new char[ len + 1 ];
    std::strcpy( out, in );

    return out;
}
