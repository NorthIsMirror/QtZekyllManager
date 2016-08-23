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

        // Try future 0, 1, 2, 3, 4 elements, forming
        // 1, 2, 3, 4, 5 elements packs
        std::vector<bool> five_equal;
        five_equal.clear();
        for( int j = 0; j < 5; j ++ ) {
            if( j+i < size ) {
                current.push_back( messages[ i + j ] );
            } else {
                five_equal.push_back( false );
                continue;
            }

            // Compare if 1-5 messages repeated
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

            five_equal.push_back( equal );
        }

        if( five_equal[4] ) {
            output.append( "<font color=green>Last 5 messages repeated</font>" );
            i += 4; // also ++ i in loop
        } else if( five_equal[3] ) {
            output.append( "<font color=green>Last 4 messages repeated</font>" );
            i += 3; // also ++ i in loop
        } else if( five_equal[2] ) {
            output.append( "<font color=green>Last 3 messages repeated</font>" );
            i += 2; // also ++ i in loop
        } else if( five_equal[1] ) {
            output.append( "<font color=green>Last 2 messages repeated</font>" );
            i += 1;
        } else if( five_equal[0] ) {
            output.append( "<font color=green>Last message repeated</font>" );
            i += 0;
        } else {
            output.append( messages[ i ] );
        }
    }

    return output;
}

