#ifndef COMMITDIALOG_H
#define COMMITDIALOG_H

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
    bool accepted() const { return accepted_; }

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    bool accepted_;
    QString commitMessage_;
    QsciLexer *lexer_;
    QFont font_;
    Ui::CommitDialog *ui;
};

#endif // COMMITDIALOG_H
