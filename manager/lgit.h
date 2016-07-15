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

    void setRepo( const QString & repo_path ) { repo_path_ = repo_path; }
    int openRepo();

signals:

public slots:

private:
    QString repo_path_;
    git_repository *repo_;
};

#endif // LGIT_H
