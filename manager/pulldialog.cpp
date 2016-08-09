#include "pulldialog.h"
#include "ui_pulldialog.h"

#include <QTableWidgetItem>
#include <QTableWidget>
#include <QVariant>
#include <QDebug>

static QDebug operator<<(QDebug out, const std::string & str)
{
    out << QString::fromStdString(str);
    return out;
}

PullDialog::PullDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PullDialog)
{
    ui->setupUi(this);
}

PullDialog::~PullDialog()
{
    delete ui;
}

int PullDialog::reset()
{
    int error, retval = 0;

    error = lgit_->loadBranches( BRANCH_LOCAL );
    error = lgit_->establishCurrent();
    error = lgit_->listRemotes();

    // What's currently checked out
    if ( lgit_->current().type() == CURRENT_TYPE_BRANCH ) {
        ui->checkoutLabel->setText( tr("Currently checked out: ") + QString::fromStdString( lgit_->current().branch() ) + tr(" (branch)") );
    } else if ( lgit_->current().type() == CURRENT_TYPE_TAG ) {
        ui->checkoutLabel->setText( tr("Currently checked out: ") + QString::fromStdString( lgit_->current().tag() ) + tr(" (tag)") );
    } else if ( lgit_->current().type() == CURRENT_TYPE_OID ) {
        ui->checkoutLabel->setText( tr("Detached HEAD points to: ") + QString::fromStdString( lgit_->current().oid() ) );
    }

    // List of branches that can be fetched
    ui->branchCombo->clear();
    int count = lgit_->branches().count();
    int remember_idx = 0;
    for ( int idx = 0; idx < count; idx ++ ) {
        const mybranch & b = lgit_->branches()[idx];
        if ( !b.is_in_fetch_head ) {
            ui->branchCombo->addItem( QString::fromStdString( b.name ), QVariant( idx ) );

            if( b.tip_sha == lgit_->current().oid() ) {
                remember_idx = idx;
            }
        }
    }
    ui->branchCombo->setCurrentIndex( remember_idx );

    // List available remotes
    ui->remotesCombo->clear();
    int count2 = lgit_->remotes().count();
    for ( int idx = 0; idx < count2; idx ++ ) {
        std::string remote = lgit_->remotes().entry(idx).name;
        ui->remotesCombo->addItem( QString::fromStdString( remote ), QVariant( idx ) );
    }

    populateFetchHead();

    // List log of selected FETCH_HEAD tip
    if( ui->fetchHeadCombo->count() > 0 ) {
        int idx = ui->fetchHeadCombo->currentData().toInt();
        const mybranch & b = lgit_->branches()[ idx ];
        logOfTip( QString::fromStdString( b.tip_sha ), QString::fromStdString( b.name ) );
    }

    return retval;
}

int PullDialog::populateFetchHead()
{
    ui->fetchHeadCombo->clear();
    int count = lgit_->branches().count();

    // List of FETCH_HEAD revisions
    for ( int idx = 0; idx < count; idx ++ ) {
        const mybranch & b = lgit_->branches()[ idx ];
        if ( b.is_in_fetch_head ) {
            QString name = QString::fromStdString( b.name );
            QString remoteUrl = QString::fromStdString( b.fetch_head_remote_url );

            myremote myremote = lgit_->remotes().findRemoteByUrl( remoteUrl );
            if( myremote.push_url != "-" ) {
                name = name + " [" + QString::fromStdString( myremote.name ) + "]";
            }
            ui->fetchHeadCombo->addItem( name, QVariant( idx ) );
        }
    }

    return 0;
}

int PullDialog::logOfTip( QString sha, QString hide )
{
    lgit_->readLog( sha, hide );
    int count = lgit_->log().count();

    ui->tableWidget->setRowCount( 0 );
    for( int idx = 0; idx < count; idx ++ ) {
        log_entry entry = lgit_->log().entry( idx );
        QTableWidgetItem *item1 = new QTableWidgetItem( QString::fromStdString( entry.sha ) );
        QTableWidgetItem *item2 = new QTableWidgetItem( QString::fromStdString( entry.author ) );
        QTableWidgetItem *item3 = new QTableWidgetItem( QString::fromStdString( entry.message ) );

        item1->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );
        item2->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );
        item3->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );

        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);

        ui->tableWidget->setItem( row, 0, item1 );
        ui->tableWidget->setItem( row, 1, item2 );
        ui->tableWidget->setItem( row, 2, item3 );
    }

    return 0;
}

void PullDialog::on_fetchHeadCombo_activated( int index )
{
    // List log of selected FETCH_HEAD tip
    if( ui->fetchHeadCombo->count() > 0 ) {
        int idx = ui->fetchHeadCombo->currentData().toInt();
        const mybranch & b = lgit_->branches()[ idx ];
        logOfTip( QString::fromStdString( b.tip_sha ), QString::fromStdString( b.name ) );
    }
}

void PullDialog::on_fetchBranch_clicked()
{
    if ( ui->branchCombo->count() <= 0 ) {
        return;
    }

    int idx = ui->branchCombo->currentData().toInt();
    const mybranch & b = lgit_->branches()[ idx ];

    int idx2 = ui->remotesCombo->currentData().toInt();
    const myremote & r = lgit_->remotes().entry( idx2 );

    QString branchArg = QString::fromStdString( b.name );
    QString remoteArg = QString::fromStdString( r.name );

    int error = lgit_->fetchBranch( branchArg, remoteArg );


}
