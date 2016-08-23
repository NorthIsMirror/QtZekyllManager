/*
 Copyright 2016 Sebastian Gniazdowski

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
