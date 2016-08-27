#ifndef LOGDIALOG_H
#define LOGDIALOG_H

#include "lgit.h"

#include <QDialog>

namespace Ui {
class LogDialog;
}

class LogDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LogDialog( QWidget *parent = 0 );
    ~LogDialog();

    void setLGit( lgit *_lgit ) { lgit_ = _lgit; }
    const QString & selectedSha() const { return selectedSha_; }
    int reset();

private slots:
    void on_buttonBox_accepted();

private:

    QString selectedSha_;

    lgit *lgit_;

    Ui::LogDialog *ui;
};

#endif // LOGDIALOG_H
