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
