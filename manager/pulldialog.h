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
    int updateMergeAnalysis();
    int switchTableToLog();
    int switchTableToNotifications();
    int logOfTip( QString sha, QString branch );
    void runCommitDialog( const std::string & msg, bool usem, const std::string & parent1, bool use1, const std::string & parent2 , bool use2 );
    void addNotification( git_checkout_notify_t why, const QString & path );

private slots:
    void on_fetchHeadCombo_activated(int index);

    void on_fetchBranch_clicked();

    void on_fetchAll_clicked();

    void on_fetchURL_clicked();

    void on_merge_clicked();

private:
    lgit *lgit_;
    lgit_branches *lgit_branches_;
    lgit_current *lgit_current_;

    bool table_has_notifications_;

    Ui::PullDialog *ui;
};

#endif // PULLDIALOG_H
