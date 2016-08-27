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

#ifndef LGIT_H
#define LGIT_H

#include "gitoperationtracker.h"
#include "lgit_branches.h"
#include "lgit_current.h"
#include "lgit_remotes.h"
#include "lgit_log.h"
#include "lgit_tags.h"
#include "lgit_status.h"

#include <tuple>

#include <QObject>
#include <QVector>
#include <QString>

#include "git2.h"

enum BackendStatus {
    NOT_INITIALIZED = 0,
    INITIALIZED = 1,
    PARTIAL_INIT_NO_DEFAULT_SIG = 2
};

enum AnalysisResult {
    ANALYSIS_EMPTY=0,
    ANALYSIS_UP_TO_DATE=1,
    ANALYSIS_FASTFORWARD=2,
    ANALYSIS_UNBORN=4,
    ANALYSIS_NONE=8,
    ANALYSIS_NORMAL=16,
    ANALYSIS_ERROR=32,
    ANALYSIS_UNSET=64
};

enum CheckoutType {
    CHECKOUT_DRY_RUN=1,
    CHECKOUT_MERGE=2,
    CHECKOUT_FORCE=3
};

enum CheckoutOperationEvent {
    CHECKOUT_OP_CONFLICT=1,
    CHECKOUT_OP_DIRTY=2,
    CHECKOUT_OP_UPDATED=4,
    CHECKOUT_OP_UNTRACKED=8,
    CHECKOUT_OP_IGNORED=16
};

struct checkout_operation_event {
    CheckoutOperationEvent type;
    std::string path;
};

class lgit : public QObject
{
    Q_OBJECT
public:
    explicit lgit(QObject *parent = 0);
    ~lgit();

    int constructorErrorCode() const { return constructor_error_code_; }
    int backendStatus() const { return backend_status_; }

    std::tuple< QString, QString, git_time, int > testDefaultSignature();

    void setRepoPath( const QString & repo_path ) { repo_path_ = repo_path; is_name_set_ = is_email_set_ = is_when_set_ = false; }

    void setName( const QString & name ) { name_ = name; is_name_set_ = true; }
    void setEmail( const QString & email ) { email_ = email; is_email_set_ = true; }
    void setWhen( const git_time & when ) { when_ = when; is_when_set_ = true; }

    const QString & name() const { return name_; }
    const QString & email() const { return email_; }
    const git_time & when() const { return when_; }

    bool is_name_and_email_set() const { return is_name_set_ && is_email_set_; }
    bool is_all_set() const { return is_name_set_ && is_email_set_ && is_when_set_; }

    int hardReset();
    int commit( const QString & message, const QVector< QString > & parents = QVector< QString > () );
    int fetchBranch( const QString & mybranch, const QString & from );
    int analyzeMerge( const std::string & target_branch, const std::string & tip_sha );
    int fastForwardSha( const std::string & target_branch, const std::string & tip_sha, CheckoutType type );
    int mergeBranch( const std::string & branch, const std::string & our_tip, const mybranch & their_fh_entry );
    int indexHasConflicts( bool *result );
    int loadBranches( int type );
    int establishCurrent();
    int listRemotes();
    int readLog( const QString & tip_sha = QString(), const QString & hide = QString(), bool equip = true );
    int listMergeHeads( std::vector<std::string> & output );
    int loadTags();
    int checkout( const std::string & target, const std::string & tip_sha, bool is_branch, bool is_tag );
    int doStatus();
    int createBranch( const std::string & name, const std::string & tip_sha = std::string() );

    AnalysisResult analysisResult() const { return analysisResult_; }

    std::vector< checkout_operation_event > & checkoutOperationData() { return checkout_operation_data_; }
    std::vector< std::string > checkoutConflicts() { return gatherCheckoutOpDataForType( CHECKOUT_OP_CONFLICT ); }
    std::vector< std::string > checkoutDirty() { return gatherCheckoutOpDataForType( CHECKOUT_OP_DIRTY ); }
    std::vector< std::string > checkoutUpdated() { return gatherCheckoutOpDataForType( CHECKOUT_OP_UPDATED ); }
    std::vector< std::string > checkoutUntracked() { return gatherCheckoutOpDataForType( CHECKOUT_OP_UNTRACKED ); }
    std::vector< std::string > checkoutIgnored() { return gatherCheckoutOpDataForType( CHECKOUT_OP_IGNORED ); }

    std::vector< std::string > gatherCheckoutOpDataForType( CheckoutOperationEvent type );

    const lgit_branches & branches() const { return git_branches_; }
    const std::vector< mybranch > & raw_branches() const { return git_branches_.raw_branches(); }

    lgit_branches & branches_rw() { return git_branches_; }
    std::vector< mybranch > & raw_branches_rw() { return git_branches_.raw_branches_rw(); }

    const lgit_current & current() const { return git_current_; }

    const lgit_remotes & remotes() const { return git_remotes_; }
    const std::vector< myremote > & raw_remotes() const { return git_remotes_.raw_remotes(); }

    const lgit_log & log() const { return git_log_; }
    const std::vector< log_entry > & raw_log() const { return git_log_.raw_log(); }

    const lgit_tags & tags() const { return git_tags_; }
    const std::vector< mytag > & raw_tags() const { return git_tags_.raw_tags(); }

    const lgit_status & status() const { return git_status_; }
    const std::string & status_summary() const { return git_status_.status_summary(); }

    GitOperationTracker *opTracker() { return op_tracker_; }
    void setOpTracker( GitOperationTracker *op_tracker ) { op_tracker_ = op_tracker; }

signals:

public slots:

private:
    int openRepo();
    int closeRepo();

private:
    int constructor_error_code_;
    int backend_status_;

    QString name_;
    QString email_;
    git_time when_;

    bool is_name_set_;
    bool is_email_set_;
    bool is_when_set_;

    AnalysisResult analysisResult_;

    QString repo_path_;
    git_repository *repo_;

    std::vector< checkout_operation_event > checkout_operation_data_;

    GitOperationTracker *op_tracker_;

    lgit_branches git_branches_;
    lgit_current git_current_;
    lgit_remotes git_remotes_;
    lgit_log git_log_;
    lgit_tags git_tags_;
    lgit_status git_status_;
};

#endif // LGIT_H
