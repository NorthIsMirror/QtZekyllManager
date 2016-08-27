#ifndef NEWBRANCHDIALOG_H
#define NEWBRANCHDIALOG_H

#include "lgit.h"

#include <QDialog>

namespace Ui {
class NewBranchDialog;
}

class NewBranchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewBranchDialog( QWidget *parent = 0 );
    ~NewBranchDialog();

    void setLGit( lgit * _lgit ) { lgit_ = _lgit; }

    const QString & branchName() const { return branch_name_; }
    const QString & commitSha() const { return commit_sha_; }
    bool useHead() const { return use_head_; }
    bool doCheckout() const { return do_checkout_; }
    bool isUnclear() const { return unclear_; }

private slots:
    void on_buttonBox_accepted();

    void on_pickButton_clicked();

private:

    QString branch_name_;
    QString commit_sha_;
    bool use_head_;
    bool unclear_;
    bool do_checkout_;

    lgit *lgit_;

    Ui::NewBranchDialog *ui;
};

#endif // NEWBRANCHDIALOG_H
