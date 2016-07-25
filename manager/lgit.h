#ifndef LGIT_H
#define LGIT_H

#include <QString>
#include <QObject>

#include "git2.h"

class lgit : public QObject
{
    Q_OBJECT
public:
    explicit lgit(QObject *parent = 0);
    ~lgit();

    void setRepoPath( const QString & repo_path ) { repo_path_ = repo_path; }
    int hardReset();
    int commit( const QString & message );

signals:

public slots:

private:
    int openRepo();
    int closeRepo();

private:
    QString repo_path_;
    git_repository *repo_;
};

#endif // LGIT_H
