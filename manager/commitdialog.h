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

private:
    QsciLexer *lexer_;
    QFont font_;
    Ui::CommitDialog *ui;
};

#endif // COMMITDIALOG_H
