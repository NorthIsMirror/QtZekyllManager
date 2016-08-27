#include "logdialog.h"
#include "ui_logdialog.h"

#include "singleton.h"
#include "messages.h"

#include <QTableWidgetItem>
#include <QTableWidget>
#include <QDebug>

#define MessagesI Singleton<Messages>::instance()

LogDialog::LogDialog( QWidget *parent ) :
    QDialog( parent ),
    lgit_( 0 ),
    ui( new Ui::LogDialog )
{
    ui->setupUi( this );
}

LogDialog::~LogDialog()
{
    delete ui;
}

int LogDialog::reset()
{
    int retval = 0;

    retval += lgit_->readLog();

    int count = lgit_->log().count();

    ui->table->setRowCount( 0 );
    for( int idx = 0; idx < count; idx ++ ) {
        log_entry entry = lgit_->log().entry( idx );
        QTableWidgetItem *item1 = new QTableWidgetItem( QString::fromStdString( entry.sha ) );
        QTableWidgetItem *item2 = new QTableWidgetItem( QString::fromStdString( entry.author ) );
        QTableWidgetItem *item3 = new QTableWidgetItem( QString::fromStdString( entry.message ) );

        item1->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );
        item2->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );
        item3->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );

        QString summary = QString::fromStdString( entry.diff_data_summary_html() );
        item1->setToolTip( summary );
        item2->setToolTip( summary );
        item3->setToolTip( summary );

        int row = ui->table->rowCount();
        ui->table->insertRow(row);

        ui->table->setItem( row, 0, item1 );
        ui->table->setItem( row, 1, item2 );
        ui->table->setItem( row, 2, item3 );
    }

    return retval;
}

void LogDialog::on_buttonBox_accepted()
{
    if ( ui->table->selectedItems().count() > 0 ) {
        int selectedRow = ui->table->currentRow();
        if ( selectedRow >= 0 ) {
            QTableWidgetItem *selectedItem = ui->table->item( selectedRow, 0 );
            if ( selectedItem ) {
                selectedSha_ = selectedItem->text().trimmed();
                MessagesI.AppendMessageT( QString( "<font color='green'>Selected commit:</font> " ) + selectedSha_ );
            }
        }
    }
}
