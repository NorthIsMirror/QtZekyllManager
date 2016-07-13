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
