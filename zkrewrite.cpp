#include "zkrewrite.h"
#include <QDebug>

ZkRewrite::ZkRewrite(QObject *parent) : QObject(parent)
{
    QObject::connect(&process_,
                     SIGNAL(finished(int, QProcess::ExitStatus)),
                     this,
                     SLOT(handleZkRewrite(int, QProcess::ExitStatus)));
}

void ZkRewrite::handleZkRewrite(int exitCode, QProcess::ExitStatus exitStatus) {
    QString buffer = static_cast<QIODevice*>(QObject::sender())->readAll();
    QStringList entries = buffer.split("\n", QString::SkipEmptyParts);

    emit result(exitCode, entries);
}

void ZkRewrite::rewrite() {
    arguments_.clear();
    arguments_ << "-qnw" << "-p" << repoPath_ << "-z" << in_zekylls_ << "-Z" << out_zekylls_;
    process_.kill();
    process_.waitForFinished(50);
    process_.start("zkrewrite", arguments_);
    process_.waitForStarted();
}
