#ifndef COMMITDIALOG_H
#define COMMITDIALOG_H

#include <QVector>
#include <QString>
#include <QFont>
#include <QDialog>
#include "Qsci/qscilexer.h"

namespace Ui {
class CommitDialog;
}

class CommitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CommitDialog(QWidget *parent = 0);
    ~CommitDialog();

    QString & commitMessage() { return commitMessage_; }
    QString & name() { return name_; }
    QString & email() { return email_; }
    bool accepted() const { return accepted_; }
    QVector< QString > & parents() { return parents_; }

    void setCommitMessage( const QString & msg );
    void setEmail( const QString & email );
    void setName( const QString & name );
    bool addParent( const std::string & parent );
    bool addParent( const QString & parent );

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    bool accepted_;
    QString commitMessage_;
    QString name_;
    QString email_;
    QVector< QString > parents_;
    QsciLexer *lexer_;
    QFont font_;
    Ui::CommitDialog *ui;
};

#endif // COMMITDIALOG_H
