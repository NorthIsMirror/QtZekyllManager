#include "myqtabwidget.h"
#include <QTabBar>

MyQTabWidget::MyQTabWidget(QWidget *parent) : QTabWidget(parent)
{

}

void MyQTabWidget::saveTheColor() {
    default_color_ = tabBar()->tabTextColor(2);
}

void MyQTabWidget::markThirdTab(bool highlight) {
    if(highlight) {
        tabBar()->setTabTextColor(2, QColor(Qt::red));
    } else {
        tabBar()->setTabTextColor(2, default_color_);
    }
}
