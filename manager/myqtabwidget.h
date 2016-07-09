#ifndef MYQTABWIDGET_H
#define MYQTABWIDGET_H

#include <QObject>
#include <QWidget>
#include <QTabWidget>
#include <QColor>
#include <QTimer>

class MyQTabWidget : public QTabWidget
{
    Q_OBJECT

public:
    MyQTabWidget(QWidget *parent = 0);
    void saveTheColor();

public slots:
    void markThirdTab(bool highlight);
    void toggleThirdTabMark();

signals:
    void thirdTabMarkingEnough();

private:
    QColor default_color_;
    bool current_third_tab_mark_;
    int mark_count_;
    QTimer timer_;
};

#endif // MYQTABWIDGET_H
