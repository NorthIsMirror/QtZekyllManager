#include "messages.h"

using namespace std;

Messages::Messages(QObject* parent)
{

}

void Messages::AppendMessageT(const QString &message) {
    messages_ += message + "\n";

    emit messagesChanged( messages_ );
}

void Messages::AppendMessageT(const QString &message, QStringList &arr) {
    QString final_message = message + "\n";
    foreach (const QString &str, arr) {
        final_message += str + "\n";
    }

    messages_ += final_message;

    emit messagesChanged( messages_ );
}

void Messages::AppendMessageT(const QString &message, vector<string> &arr) {
    QString final_message = message + "\n";
    foreach (const string &str, arr) {
        final_message += QString::fromStdString(str) + "\n";
    }

    messages_ += final_message;

    emit messagesChanged( messages_ );
}
