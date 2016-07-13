#ifndef MYQTABLEWIDGET_H
#define MYQTABLEWIDGET_H

#include <QObject>
#include <QWidget>
#include <QTableWidget>
#include <tuple>

class MyQTableWidget : public QTableWidget
{
    Q_OBJECT
public:
    MyQTableWidget(QWidget *parent = 0);

    std::tuple<int,int> move(bool up);
    QList<QTableWidgetItem*> takeRow(int row);
    void setRow(int row, const QList<QTableWidgetItem*>& rowItems);

    int maximumId();
};

#endif // MYQTABLEWIDGET_H
