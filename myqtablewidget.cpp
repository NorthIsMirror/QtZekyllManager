#include "myqtablewidget.h"

MyQTableWidget::MyQTableWidget(QWidget *parent) : QTableWidget(parent)
{

}

void MyQTableWidget::move(bool up)
{
    if( selectedItems().count() == 0 ) {
        return;
    }
    const int sourceRow = row(selectedItems().at(0));
    const int destRow = (up ? sourceRow-1 : sourceRow+1);

    if( destRow < 0 || destRow >= rowCount() ) {
        return;
    }

    // take whole rows
    QList<QTableWidgetItem*> sourceItems = takeRow(sourceRow);
    QList<QTableWidgetItem*> destItems = takeRow(destRow);

    // set back in reverse order
    setRow(sourceRow, destItems);
    setRow(destRow, sourceItems);

    selectRow(destRow);
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
