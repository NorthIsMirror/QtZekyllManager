#include "messages.h"
#include <QVector>

using namespace std;

Messages::Messages(QObject* parent)
{

}

void Messages::AppendMessageT(const QString &message) {
    messages_ += message + "\n";

    emit messagesChanged( GetMessagesNoRepeat(messages_) );
}

void Messages::AppendMessageT(const QString &message, QStringList &arr) {
    QString final_message = message + "\n";
    foreach( const QString &str, arr ) {
        final_message += str + "\n";
    }

    messages_ += final_message;

    emit messagesChanged( GetMessagesNoRepeat(messages_) );
}

void Messages::AppendMessageT(const QString &message, vector<string> &arr) {
    QString final_message = message + "\n";
    foreach( const string &str, arr ) {
        final_message += QString::fromStdString(str) + "\n";
    }

    messages_ += final_message;

    emit messagesChanged( GetMessagesNoRepeat(messages_) );
}

QStringList Messages::GetMessagesNoRepeat( const QStringList & _messages )
{
    QStringList output;
    QVector<QString> messages = QVector<QString>::fromList( _messages );
    int size = messages.count();
    for( int i = 0; i < size; i ++ ) {
        std::vector<QString> current;
        current.clear();

        // Try future 0, 1, 2, 3 elements, forming
        // 1, 2, 3, 4 elements packs
        std::vector<bool> four_equal;
        four_equal.clear();
        for( int j = 0; j < 4; j ++ ) {
            if( j+i < size ) {
                current.push_back( messages[ i + j ] );
            } else {
                four_equal.push_back( false );
                continue;
            }

            // Compare if 1-4 messages repeated
            int k=1;
            bool equal = true;
            for( std::vector<QString>::reverse_iterator rit = current.rbegin(); rit != current.rend(); ++ rit ) {
                // Not enough messages before, thus there cannot be repetition
                if( i-k < 0 ) {
                    equal = false;
                    break;
                }

                // k-th previous message equal to l-th
                if( messages[i-k] != *rit ) {
                    equal = false;
                    break;
                }

                k ++;
            }

            four_equal.push_back( equal );
        }

        if( four_equal[3] ) {
            output.append( "<font color=green>Last 4 messages repeated</font>" );
            i += 3; // also ++ i in loop
        } else if( four_equal[2] ) {
            output.append( "<font color=green>Last 3 messages repeated</font>" );
            i += 2; // also ++ i in loop
        } else if( four_equal[1] ) {
            output.append( "<font color=green>Last 2 messages repeated</font>" );
            i += 1;
        } else if( four_equal[0] ) {
            output.append( "<font color=green>Last message repeated</font>" );
            i += 0;
        } else {
            output.append( messages[ i ] );
        }
    }

    return output;
}

