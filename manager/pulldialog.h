#ifndef PULLDIALOG_H
#define PULLDIALOG_H

#include "lgit.h"
#include "lgit_branches.h"
#include "lgit_current.h"

#include <QDialog>

namespace Ui {
class PullDialog;
}

class PullDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PullDialog(QWidget *parent = 0);
    ~PullDialog();

    void setLGit( lgit *_lgit ) { lgit_ = _lgit; }
    int reset();
    int populateFetchHead();
    int logOfTip( QString sha, QString branch );

private slots:
    void on_fetchHeadCombo_activated(int index);

    void on_fetchBranch_clicked();

    void on_fetchAll_clicked();

    void on_fetchURL_clicked();

private:
    lgit *lgit_;
    lgit_branches *lgit_branches_;
    lgit_current *lgit_current_;

    Ui::PullDialog *ui;
};

#endif // PULLDIALOG_H
