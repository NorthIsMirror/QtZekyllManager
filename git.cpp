#include "Git.h"
#include <QDebug>

Git::Git(QObject *parent) : QObject(parent)
{
    QObject::connect(&process_,
                     SIGNAL(finished(int, QProcess::ExitStatus)),
                     this,
                     SLOT(handleGit_remove_lzcsde(int, QProcess::ExitStatus)));
    process_.setProcessChannelMode( QProcess::MergedChannels );
}

void Git::handleGit_remove_lzcsde(int exitCode, QProcess::ExitStatus exitStatus) {
    QString buffer = static_cast<QIODevice*>(QObject::sender())->readAll();
    QStringList entries = buffer.split("\n", QString::SkipEmptyParts);

    emit result_git_rm(exitCode, entries);
}

void Git::remove_lzcsde(LZCSDE lzcsde) {
    const QVector<LZCSDE_Entry> & entries = lzcsde.entries();
    int size = entries.count();
    for( int i=0; i<size; i++ ) {
        arguments_remove_.clear();
        arguments_remove_ << "-C" << repoPath_ << "rm" << "-f" << entries[i].listing();
        process_.kill();
        process_.waitForFinished(50);
        process_.start("git", arguments_remove_);
        process_.waitForStarted();
        process_.waitForFinished();
    }
}
