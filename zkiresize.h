#ifndef ZKIRESIZE_H
#define ZKIRESIZE_H

#include <QObject>
#include <QProcess>

class ZkIResize : public QObject
{
    Q_OBJECT
public:
    explicit ZkIResize(QObject *parent = 0);
    void setIndex( int index ) { index_ = index; }
    void setRepoPath( const QString & path ) { repoPath_ = path; }

signals:
    void result_list(int exitCode, QStringList entries);
    void result_consistent(int exitCode, QStringList entries);
    void result_resize(int exitCode, QStringList entries);

public slots:

    void handleZkIResizeList(int exitCode, QProcess::ExitStatus exitStatus);
    void handleZkIResizeConsistent(int exitCode, QProcess::ExitStatus exitStatus);
    void handleZkIResizeResize(int exitCode, QProcess::ExitStatus exitStatus);
    void list();
    void consistent();
    void resize(int current_size, int new_size);

    void waitForFinishedList() { process_list_.waitForFinished(); }
    void waitForFinishedConsistent() { process_consistent_.waitForFinished(); }
    void waitForFinishedResize() { process_resize_.waitForFinished(); }

private:
    int index_;
    QString repoPath_;
    QProcess process_list_;
    QProcess process_consistent_;
    QProcess process_resize_;
    QStringList arguments_list_;
    QStringList arguments_consistent_;
    QStringList arguments_resize_;
};

#endif // ZKIRESIZE_H
