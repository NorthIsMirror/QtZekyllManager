#include "myqtabwidget.h"
#include <QTabBar>

MyQTabWidget::MyQTabWidget(QWidget *parent) : QTabWidget(parent), current_third_tab_mark_(false), mark_count_(0)
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

void MyQTabWidget::toggleThirdTabMark() {
    current_third_tab_mark_ = !current_third_tab_mark_;
    markThirdTab( current_third_tab_mark_ );

    if( mark_count_ == 1 ) {
        mark_count_ = 0;
    } else if ( mark_count_ == 0 ) {
        mark_count_ = 3;
    } else {
        mark_count_ --;
    }

    if( mark_count_ == 0 ) {
        emit thirdTabMarkingEnough();
    }
}
