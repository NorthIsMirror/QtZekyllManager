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

#ifndef ZKREWRITE_H
#define ZKREWRITE_H

#include <QObject>
#include <QProcess>

class ZkRewrite : public QObject
{
    Q_OBJECT
public:
    explicit ZkRewrite(QObject *parent = 0);
    void setInZekylls( const QString & zekylls ) { in_zekylls_ = zekylls; }
    void setOutZekylls( const QString & zekylls ) { out_zekylls_ = zekylls; }
    void setRepoPath( const QString & path ) { repoPath_ = path; }

signals:
    void result( int exitCode, QStringList reply );

public slots:

    void handleZkRewrite( int exitCode, QProcess::ExitStatus exitStatus );
    void rewrite();
    void waitForFinishedRewrite() { process_.waitForFinished(); }

private:
    QString in_zekylls_;
    QString out_zekylls_;
    QString repoPath_;
    QProcess process_;
    QStringList arguments_;
};

#endif // ZKIREWRITE_H
