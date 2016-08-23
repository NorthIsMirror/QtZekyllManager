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
