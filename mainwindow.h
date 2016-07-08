#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include "zkiresize.h"
#include "lzcsde.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    ZkIResize *zkiresize_;
    LZCSDE lzcsde_list_;
    LZCSDE lzcsde_consistent_;

    void insertLZCSDTableRow(QTableWidget * tableWidget, const QString & zekyll, bool checked, const QString & section, const QString & description);
    void insertLZSDETableRow(QTableWidget * tableWidget, const QString & zekyll, const QString & section,
                                const QString & description, const QString & error);

public slots:
    void handle_zkiresize_list(int exitCode, QStringList entries);
    void handle_zkiresize_consistent(int exitCode, QStringList entries);
};

#endif // MAINWINDOW_H
