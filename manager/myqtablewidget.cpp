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

#include "myqtablewidget.h"

using namespace std;

MyQTableWidget::MyQTableWidget(QWidget *parent) : QTableWidget(parent)
{

}

tuple<int,int> MyQTableWidget::move(bool up)
{
    if( selectedItems().count() == 0 ) {
        return make_tuple(-1, -1);
    }

    const int sourceRow = row(selectedItems().at(0));
    const int destRow = (up ? sourceRow-1 : sourceRow+1);

    if( destRow < 0 || destRow >= rowCount() ) {
        return make_tuple(-1, -1);
    }

    // take whole rows
    QList<QTableWidgetItem*> sourceItems = takeRow(sourceRow);
    QList<QTableWidgetItem*> destItems = takeRow(destRow);

    // set back in reverse order
    setRow(sourceRow, destItems);
    setRow(destRow, sourceItems);

    selectRow(destRow);

    bool ok = false;
    int sourceId = sourceItems.first()->text().toInt(&ok);
    if(!ok) {
        return make_tuple(-2, -2);
    }

    int destId = destItems.first()->text().toInt(&ok);
    if(!ok) {
        return make_tuple(-2,-2);
    }

    return make_tuple(sourceId, destId);
}

// takes and returns the whole row
QList<QTableWidgetItem*> MyQTableWidget::takeRow(int row)
{
    QList<QTableWidgetItem*> rowItems;
    for (int col = 0; col < columnCount(); ++col)
    {
        rowItems << takeItem(row, col);
    }
    return rowItems;
}

// sets the whole row
void MyQTableWidget::setRow(int row, const QList<QTableWidgetItem*>& rowItems)
{
    for (int col = 0; col < columnCount(); ++col)
    {
        setItem(row, col, rowItems.at(col));
    }
}

int MyQTableWidget::maximumId() {
    int rcount = rowCount();
    int max = 0;
    for( int i=0; i<rcount; i++ ) {
        QString text = item(i,0)->text();
        bool ok=false;
        int id = text.toInt(&ok);
        if(!ok) {
            continue;
        }
        if( max < id ) {
            max = id;
        }
    }
    return max;
}
