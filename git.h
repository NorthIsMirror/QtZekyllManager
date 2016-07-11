#ifndef GIT_H
#define GIT_H

#include <QObject>
#include <QProcess>
#include "lzcsde.h"

class Git : public QObject
{
    Q_OBJECT
public:
    explicit Git(QObject *parent = 0);
    void setRepoPath( const QString & path ) { repoPath_ = path; }

signals:
    void result_git_rm( int exitCode, QStringList reply );

public slots:

    void handleGit_remove_lzcsde( int exitCode, QProcess::ExitStatus exitStatus );
    void remove_lzcsde(LZCSDE entries);
    void waitForFinishedRemove() { process_.waitForFinished(); }

private:
    QString repoPath_;
    QProcess process_;
    QStringList arguments_remove_;
};

#endif // GIT_H
