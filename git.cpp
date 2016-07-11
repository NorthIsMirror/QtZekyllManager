#include "Git.h"
#include <QDebug>

Git::Git(QObject *parent) : QObject(parent)
{
    QObject::connect(&process_remove_,
                     SIGNAL(finished(int, QProcess::ExitStatus)),
                     this,
                     SLOT(handleGit_remove_lzcsde(int, QProcess::ExitStatus)));
    process_remove_.setProcessChannelMode( QProcess::MergedChannels );

    QObject::connect(&process_rename_,
                     SIGNAL(finished(int, QProcess::ExitStatus)),
                     this,
                     SLOT(handleGit_rename_lzcsde(int, QProcess::ExitStatus)));
    process_rename_.setProcessChannelMode( QProcess::MergedChannels );
}

void Git::handleGit_remove_lzcsde(int exitCode, QProcess::ExitStatus exitStatus) {
    QString buffer = static_cast<QIODevice*>(QObject::sender())->readAll();
    QStringList entries = buffer.split("\n", QString::SkipEmptyParts);

    emit result_git_rm(exitCode, entries);
}

void Git::handleGit_rename_lzcsde(int exitCode, QProcess::ExitStatus exitStatus) {
    QString buffer = static_cast<QIODevice*>(QObject::sender())->readAll();
    QStringList entries = buffer.split("\n", QString::SkipEmptyParts);

    emit result_git_mv(exitCode, entries);
}

void Git::remove_lzcsde(LZCSDE lzcsde) {
    const QVector<LZCSDE_Entry> & entries = lzcsde.entries();
    int size = entries.count();
    for( int i=0; i<size; i++ ) {
        arguments_remove_.clear();
        arguments_remove_ << "-C" << repoPath_ << "rm" << "-f" << entries[i].listing();
        process_remove_.kill();
        process_remove_.waitForFinished(50);
        process_remove_.start("git", arguments_remove_);
        process_remove_.waitForStarted();
        process_remove_.waitForFinished();
    }
}

void Git::rename_lzcsde_to_lzcsde( std::pair<LZCSDE, LZCSDE> from_to ) {
    const QVector<LZCSDE_Entry> & from = from_to.first.entries();
    const QVector<LZCSDE_Entry> & to = from_to.second.entries();

    if( from.count() != to.count() ) {
        return;
    }

    int size = from.count();
    for( int i=0; i<size; i++ ) {
        QStringList target_filename;
        target_filename << to[i].zekyll() << "." << to[i].section() << "--" << to[i].description();
        arguments_rename_.clear();
        arguments_rename_ << "-C" << repoPath_ << "mv" << from[i].listing() << target_filename.join("");
        process_rename_.kill();
        process_rename_.waitForFinished(50);
        process_rename_.start("git", arguments_rename_);
        process_rename_.waitForStarted();
        process_rename_.waitForFinished();
    }
}
