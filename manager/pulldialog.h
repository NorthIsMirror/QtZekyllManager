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

#ifndef PULLDIALOG_H
#define PULLDIALOG_H

#include "lgit.h"
#include "lgit_branches.h"
#include "lgit_current.h"
#include "progresswidget.h"

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
    void updateProgress( double progress );

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

    bool progressShown_;
    ProgressWidget *pwidget_;

    Ui::PullDialog *ui;
};

#endif // PULLDIALOG_H
