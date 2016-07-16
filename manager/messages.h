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
