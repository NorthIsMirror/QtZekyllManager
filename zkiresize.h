#ifndef ZKIRESIZE_H
#define ZKIRESIZE_H

#include <QObject>
#include <QProcess>

class ZkIResize : public QObject
{
    Q_OBJECT
public:
    explicit ZkIResize(QObject *parent = 0);

signals:
    void result_list(int exitCode, QStringList entries);
    void result_consistent(int exitCode, QStringList entries);

public slots:

    void handleZkIResizeList(int exitCode, QProcess::ExitStatus exitStatus);
    void handleZkIResizeConsistent(int exitCode, QProcess::ExitStatus exitStatus);
    void list();
    void consistent();

private:
    QProcess process_list_;
    QProcess process_consistent_;
    QStringList arguments_list_;
    QStringList arguments_consistent_;
};

#endif // ZKIRESIZE_H
