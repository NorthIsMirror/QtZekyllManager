/*
 Copyright 2016 Sebastian Gniazdowski

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "zkiresize.h"
#include "singleton.h"
#include "messages.h"
#include <QDebug>

#define MessagesI Singleton<Messages>::instance()

ZkIResize::ZkIResize(QObject *parent) : QObject(parent)
{
    QObject::connect(&process_list_,
                     SIGNAL(finished(int, QProcess::ExitStatus)),
                     this,
                     SLOT(handleZkIResizeList(int, QProcess::ExitStatus)));

    process_list_.setProcessChannelMode( QProcess::MergedChannels );

    QObject::connect(&process_consistent_,
                     SIGNAL(finished(int, QProcess::ExitStatus)),
                     this,
                     SLOT(handleZkIResizeConsistent(int, QProcess::ExitStatus)));

    process_consistent_.setProcessChannelMode( QProcess::MergedChannels );

    QObject::connect(&process_resize_,
                     SIGNAL(finished(int, QProcess::ExitStatus)),
                     this,
                     SLOT(handleZkIResizeResize(int, QProcess::ExitStatus)));

    process_resize_.setProcessChannelMode( QProcess::MergedChannels );
}

void ZkIResize::handleZkIResizeList(int exitCode, QProcess::ExitStatus exitStatus) {
    QString buffer = static_cast<QIODevice*>(QObject::sender())->readAll();
    QStringList entries = buffer.split("\n", QString::SkipEmptyParts);

    if( exitCode == 12 ) {
        consistent();
    }

    emit result_list(exitCode, entries);
}

void ZkIResize::handleZkIResizeConsistent(int exitCode, QProcess::ExitStatus exitStatus) {
    QString buffer = static_cast<QIODevice*>(QObject::sender())->readAll();
    QStringList entries = buffer.split("\n", QString::SkipEmptyParts);

    emit result_consistent(exitCode, entries);
}

void ZkIResize::handleZkIResizeResize(int exitCode, QProcess::ExitStatus exitStatus) {
    QString buffer = static_cast<QIODevice*>(QObject::sender())->readAll();
    QStringList entries = buffer.split("\n", QString::SkipEmptyParts);

    emit result_resize(exitCode, entries);
}

void ZkIResize::list() {
    arguments_list_.clear();
    arguments_list_ << "-qn" << "--req" << "-p" << repoPath_ << "-i" << QString( "%1" ).arg( index_ ) << "--list";
    //qDebug() << "List: " << arguments_list_;
    process_list_.kill();
    process_list_.waitForFinished(50);
    process_list_.start("zkiresize", arguments_list_);
    if(!process_list_.waitForStarted()) {
        MessagesI.AppendMessageT("<font color=red>Failed to communicate with the zekyll backend (zkiresize) for list operation</font>");
    }
}

void ZkIResize::consistent() {
    arguments_consistent_.clear();
    arguments_consistent_ << "-qn" << "--req" << "-p" << repoPath_ << "-i" << "1" << "--consistent";
    //qDebug() << "Consistent: " << arguments_consistent_;
    process_consistent_.kill();
    process_consistent_.waitForFinished(50);
    process_consistent_.start("zkiresize", arguments_consistent_);
    if(!process_consistent_.waitForStarted()) {
        MessagesI.AppendMessageT("<font color=red>Failed to communicate with the zekyll backend (zkiresize)</font>");
    }
}

void ZkIResize::resize(int current_size, int new_size) {
    if( current_size == new_size ) {
        return;
    }

    arguments_resize_.clear();
    arguments_resize_ << "-qnw" << "--req" << "-p" << repoPath_ << "-i" << QString("%1").arg(index_) << "-s" << QString("%1").arg(new_size);
    arguments_resize_ << "--section" << "Z" << "--desc" << "New Zekyll";
    //qDebug() << "Resize: " << arguments_resize_;
    process_resize_.kill();
    process_resize_.waitForFinished(50);
    process_resize_.start("zkiresize", arguments_resize_);
    if(!process_resize_.waitForStarted()) {
        MessagesI.AppendMessageT("<font color=red>Failed to communicate with the zekyll backend (zkiresize) for resize operation</font>");
    }
}
