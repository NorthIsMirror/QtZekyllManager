#ifndef MYQTABWIDGET_H
#define MYQTABWIDGET_H

#include <QObject>
#include <QWidget>
#include <QTabWidget>
#include <QColor>

class MyQTabWidget : public QTabWidget
{
    Q_OBJECT

public:
    MyQTabWidget(QWidget *parent = 0);
    void saveTheColor();

public slots:
    void markThirdTab(bool highlight);

private:
    QColor default_color_;
};

#endif // MYQTABWIDGET_H
