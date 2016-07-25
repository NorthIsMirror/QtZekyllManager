#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("Zekyll");
    a.setOrganizationDomain("zekyll.org");
    a.setApplicationName("Qt Zekyll Manager");
    MainWindow w;
    w.show();

    return a.exec();
}
