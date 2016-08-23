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

#ifndef MYQLINEEDIT_H
#define MYQLINEEDIT_H

#include <QObject>
#include <QLineEdit>

class MyQLineEdit : public QLineEdit
{
    Q_OBJECT

protected:
    void focusInEvent( QFocusEvent* e );

public:
    MyQLineEdit( QWidget *parent = 0 );
};

#endif // MYQLINEEDIT_H
