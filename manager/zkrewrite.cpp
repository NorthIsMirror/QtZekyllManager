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

#include "zkrewrite.h"
#include <QDebug>

ZkRewrite::ZkRewrite(QObject *parent) : QObject(parent)
{
    QObject::connect(&process_,
                     SIGNAL(finished(int, QProcess::ExitStatus)),
                     this,
                     SLOT(handleZkRewrite(int, QProcess::ExitStatus)));

    process_.setProcessChannelMode( QProcess::MergedChannels );
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
