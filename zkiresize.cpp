#include "zkiresize.h"
#include <QDebug>

ZkIResize::ZkIResize(QObject *parent) : QObject(parent)
{
    arguments_list_ << "-qn" << "-r" << "psprint" << "-i" << "1" << "--list";
    arguments_consistent_ << "-qn" << "-r" << "psprint" << "-i" << "1" << "--consistent";

    QObject::connect(&process_list_,
                     SIGNAL(finished(int, QProcess::ExitStatus)),
                     this,
                     SLOT(handleZkIResizeList(int, QProcess::ExitStatus)));

    QObject::connect(&process_consistent_,
                     SIGNAL(finished(int, QProcess::ExitStatus)),
                     this,
                     SLOT(handleZkIResizeConsistent(int, QProcess::ExitStatus)));
}

void ZkIResize::handleZkIResizeList(int exitCode, QProcess::ExitStatus exitStatus) {
    qDebug() << exitCode;
    qDebug() << exitStatus;
    QString buffer = static_cast<QIODevice*>(QObject::sender())->readAll();
    QStringList entries = buffer.split("\n");

    if( exitCode != 0 ) {
        consistent();
    }
    emit result_list(exitCode, entries);
}

void ZkIResize::handleZkIResizeConsistent(int exitCode, QProcess::ExitStatus exitStatus) {
    qDebug() << "Consistent" << exitCode;
    qDebug() << "Consistent" << exitStatus;
    QString buffer = static_cast<QIODevice*>(QObject::sender())->readAll();
    QStringList entries = buffer.split("\n");

    emit result_consistent(exitCode, entries);
}

void ZkIResize::list() {
    process_list_.start("zkiresize", arguments_list_);
}

void ZkIResize::consistent() {
    process_consistent_.start("zkiresize", arguments_consistent_);
}
