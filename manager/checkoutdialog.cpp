#include "checkoutdialog.h"
#include "ui_checkoutdialog.h"

#include <QFont>

CheckoutDialog::CheckoutDialog( QWidget *parent ) :
    QDialog( parent ),
    inserting_branches_( false ),
    inserting_tags_( false ),
    ui( new Ui::CheckoutDialog )
{
    ui->setupUi( this );
}

CheckoutDialog::~CheckoutDialog()
{
    delete ui;
}

void CheckoutDialog::addNotification( git_checkout_notify_t why, const QString & path )
{

}

int CheckoutDialog::addBranch( const QString & name, const QString & sha_tip )
{
    if( startBranches() ) {
        return 1;
    }

    ui->list->addItem( name );
    int rows = ui->list->count();
    ui->list->item( rows - 1 )->setToolTip( "<h3>" + sha_tip + "</h3>" );

    return 0;
}

int CheckoutDialog::addTag( const QString & name, const QString & sha_tip )
{
    if( startTags() ) {
        return 1;
    }

    ui->list->addItem( name );
    int rows = ui->list->count();
    ui->list->item( rows - 1 )->setToolTip( "<h3>" + sha_tip + "</h3>" );

    return 0;
}

int CheckoutDialog::addNonselectable( const QString & title )
{
    QListWidgetItem *item = new QListWidgetItem( title );
    if ( !item ) {
        return 1;
    }

    QFont font = item->font();
    font.setBold( true );
    item->setFont( font );
    item->setFlags( Qt::ItemIsEnabled );
    ui->list->addItem( item );

    return 0;
}

int CheckoutDialog::startBranches()
{
    if( inserting_branches_ ) {
        return 0;
    }

    if( addNonselectable( tr( "Branches:" ) ) ) {
        return 1;
    }

    inserting_branches_ = true;
    inserting_tags_ = false;

    return 0;
}

int CheckoutDialog::startTags()
{
    if( inserting_tags_ ) {
        return 0;
    }

    if( addNonselectable( tr( "Tags:" ) ) ) {
        return 1;
    }

    inserting_branches_ = false;
    inserting_tags_ = true;

    return 0;
}
