#ifndef LGIT_H
#define LGIT_H

#include "lgit_branches.h"
#include "lgit_current.h"
#include "lgit_remotes.h"
#include "lgit_log.h"

#include <tuple>

#include <QObject>
#include <QString>

#include "git2.h"

enum BackendStatus {
    NOT_INITIALIZED = 0,
    INITIALIZED = 1,
    PARTIAL_INIT_NO_DEFAULT_SIG = 2
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
    int commit( const QString & message );
    int fetchBranch( const QString & mybranch, const QString & from );
    int mergeBranch( const QString & mybranch );
    int loadBranches( int type );
    int establishCurrent();
    int listRemotes();
    int readLog( const QString & tip_sha, const QString & hide );

    const lgit_branches & branches() const { return git_branches_; }
    const std::vector< mybranch > & raw_branches() const { return git_branches_.raw_branches(); }

    const lgit_current & current() const { return git_current_; }

    const lgit_remotes & remotes() const { return git_remotes_; }
    const std::vector< myremote > & raw_remotes() const { return git_remotes_.raw_remotes(); }

    const lgit_log & log() const { return git_log_; }
    const std::vector< log_entry > & raw_log() const { return git_log_.raw_log(); }

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

    QString repo_path_;
    git_repository *repo_;

    lgit_branches git_branches_;
    lgit_current git_current_;
    lgit_remotes git_remotes_;
    lgit_log git_log_;
};

#endif // LGIT_H
