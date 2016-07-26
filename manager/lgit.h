#ifndef LGIT_H
#define LGIT_H

#include <tuple>
#include <QString>
#include <QObject>

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

    void setRepoPath( const QString & repo_path ) { repo_path_ = repo_path; }

    void setName( const QString & name ) { name_ = name; }
    void setEmail( const QString & email ) { email_ = email; }
    void setWhen( const git_time & when ) { when_ = when; }

    const QString & name() const { return name_; }
    const QString & email() const { return email_; }
    const git_time & when() const { return when_; }

    int hardReset();
    int commit( const QString & message );

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

    QString repo_path_;
    git_repository *repo_;
};

#endif // LGIT_H
