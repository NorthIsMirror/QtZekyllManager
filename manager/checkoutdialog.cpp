#include "checkoutdialog.h"
#include "ui_checkoutdialog.h"

#include <QFont>

struct MyRefData {
    QString name;
    QString tip_sha;
    bool is_branch;
    bool is_tag;

    MyRefData() : is_branch( false ), is_tag( false )
    { }

    MyRefData( const QString & _name, const QString & _tip_sha, bool _is_branch, bool _is_tag ) :
        name( _name ), tip_sha( _tip_sha ), is_branch( _is_branch ), is_tag( _is_tag )
    { }
};

Q_DECLARE_METATYPE( MyRefData );

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

int CheckoutDialog::addBranch( const QString & name, const QString & tip_sha )
{
    if( startBranches() ) {
        return 1;
    }

    ui->list->addItem( name );
    int rows = ui->list->count();
    QListWidgetItem *item = ui->list->item( rows - 1 );
    item->setToolTip( "<h3>" + tip_sha + "</h3>" );

    MyRefData refData( name, tip_sha, true, false );
    QVariant vRefData = QVariant::fromValue( refData );
    item->setData( Qt::UserRole, vRefData );

    return 0;
}

int CheckoutDialog::addTag( const QString & name, const QString & tip_sha )
{
    if( startTags() ) {
        return 1;
    }

    ui->list->addItem( name );
    int rows = ui->list->count();
    QListWidgetItem *item = ui->list->item( rows - 1 );
    item->setToolTip( "<h3>" + tip_sha + "</h3>" );

    MyRefData refData( name, tip_sha, false, true );
    QVariant vRefData = QVariant::fromValue( refData );
    item->setData( Qt::UserRole, vRefData );

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
