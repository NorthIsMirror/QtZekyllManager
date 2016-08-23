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

#ifndef MESSAGES_H
#define MESSAGES_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <vector>
#include <string>

class Messages : public QObject
{
    Q_OBJECT

    // The messages are gathered here
    QStringList messages_;

public:
    Messages(QObject* parent = 0);
    void AppendMessageT(const QString & message);
    void AppendMessageT(const QString & message, QStringList & arr);
    void AppendMessageT(const QString & message, std::vector<std::string> & arr);

private:
    QStringList GetMessagesNoRepeat( const QStringList & _messages );

signals:
    void messagesChanged( const QStringList & messages );
};

#endif // MESSAGES_H
