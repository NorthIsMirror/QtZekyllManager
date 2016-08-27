#include "checkoutdialog.h"
#include "ui_checkoutdialog.h"

#include <QFont>
#include <QPushButton>
#include <QDebug>

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
    lgit_( 0 ),
    ui( new Ui::CheckoutDialog )
{
    ui->setupUi( this );
    ui->label->setText( "" );
}

CheckoutDialog::~CheckoutDialog()
{
    delete ui;
}

void CheckoutDialog::addNotification( git_checkout_notify_t why, const QString & path )
{
    QTableWidgetItem *item1;

    if ( why == GIT_CHECKOUT_NOTIFY_NONE ) {
            return;
    }

    if ( ui->stackedWidget->currentIndex() != 1 ) {
        ui->stackedWidget->setCurrentIndex( 1 );
    }

    switch( why ) {
    case GIT_CHECKOUT_NOTIFY_CONFLICT:
        item1 = new QTableWidgetItem( "Conflict" );
        item1->setBackground( Qt::yellow );
        break;
    case GIT_CHECKOUT_NOTIFY_DIRTY:
        item1 = new QTableWidgetItem( "Dirty" );
        item1->setBackground( Qt::red );
        break;
    case GIT_CHECKOUT_NOTIFY_UPDATED:
        item1 = new QTableWidgetItem( "Updated" );
        item1->setBackground( Qt::green );
        break;
    case GIT_CHECKOUT_NOTIFY_UNTRACKED:
        item1 = new QTableWidgetItem( "Untracked" );
        break;
    case GIT_CHECKOUT_NOTIFY_IGNORED:
        item1 = new QTableWidgetItem( "Ignored" );
        item1->setForeground( Qt::blue );
        break;
    default:
        item1 = new QTableWidgetItem( "" );
        break;
    }

    QTableWidgetItem *item2 = new QTableWidgetItem( path );

    int row = ui->table->rowCount();
    ui->table->insertRow(row);

    ui->table->setItem( row, 0, item1 );
    ui->table->setItem( row, 1, item2 );
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

void CheckoutDialog::setCurrent( const QString & current )
{
    current_ = current;
    ui->currentLabel->setText( tr( "Current HEAD: " ) + current );
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

void CheckoutDialog::on_buttonBox_clicked( QAbstractButton *button )
{
    int error;

    if( button == static_cast< QAbstractButton * > ( ui->buttonBox->button( QDialogButtonBox::Apply ) ) ) {
        if ( !lgit_ ) {
            return;
        }

        if ( ui->list->selectedItems().count() == 0 ) {
            ui->label->setText( tr("No branch or tag selected") );
        } else {
            QListWidgetItem *item = ui->list->selectedItems().first();
            MyRefData refData = item->data( Qt::UserRole ).value<MyRefData>();

            // Clear notifications table
            ui->table->setRowCount( 0 );

            // Refresh branches to allow right-before-action verification of tip sha
            lgit_->loadBranches( BRANCH_LOCAL );

            if ( ( error = lgit_->checkout( refData.name.toStdString(), refData.tip_sha.toStdString(), refData.is_branch, refData.is_tag ) ) > 0 ) {
                ui->label->setText( tr( "Problems with checkout, error code: %1" ).arg( error ) );
                ui->currentLabel->setText( tr( "Current HEAD: unknown" ) );
            } else {
                ui->label->setText( tr( "Checkout successful" ) );
                ui->currentLabel->setText( tr( "Current HEAD: " ) + refData.name );
            }
        }
    }
}
