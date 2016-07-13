#ifndef GIT_H
#define GIT_H

#include <QObject>
#include <QProcess>
#include <utility>
#include "lzcsde.h"

class Git : public QObject
{
    Q_OBJECT
public:
    explicit Git(QObject *parent = 0);
    void setRepoPath( const QString & path ) { repoPath_ = path; }

signals:
    void result_git_rm( int exitCode, QStringList reply );
    void result_git_mv( int exitCode, QStringList reply );

public slots:

    void handleGit_remove_lzcsde( int exitCode, QProcess::ExitStatus exitStatus );
    void handleGit_rename_lzcsde( int exitCode, QProcess::ExitStatus exitStatus );
    void remove_lzcsde(LZCSDE entries);
    void rename_lzcsde_to_lzcsde( std::pair<LZCSDE, LZCSDE> from_to );
    void waitForFinishedRemove() { process_remove_.waitForFinished(); }
    void waitForFinishedRename() { process_rename_.waitForFinished(); }

private:
    QString repoPath_;
    QProcess process_remove_;
    QStringList arguments_remove_;

    QProcess process_rename_;
    QStringList arguments_rename_;
};

#endif // GIT_H
