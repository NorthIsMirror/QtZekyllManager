#include "pulldialog.h"
#include "ui_pulldialog.h"

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

int PullDialog::prepare()
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
    for ( int i = 0; i < count; i ++ ) {
        const mybranch & b = lgit_->branches()[i];
        if ( !b.is_in_fetch_head ) {
            ui->branchCombo->addItem( QString::fromStdString( b.name ) );

            if( b.tip_sha == lgit_->current().oid() ) {
                remember_idx = i;
            }
        }
    }
    ui->branchCombo->setCurrentIndex( remember_idx );

    // List of FETCH_HEAD revisions
    for ( int idx = 0; idx < count; idx ++ ) {
        const mybranch & b = lgit_->branches()[ idx ];
        if ( b.is_in_fetch_head ) {
            QString name = QString::fromStdString( b.name );
            QString remoteUrl = QString::fromStdString( b.fetch_head_remote_url );

            remote myremote = lgit_->remotes().findRemoteByUrl( remoteUrl );
            if( myremote.push_url != "-" ) {
                name = name + " [" + QString::fromStdString( myremote.name ) + "]";
            }
            ui->fetchHeadCombo->addItem( name, QVariant( idx ) );
        }
    }

    // List log of selected FETCH_HEAD tip
    if( ui->fetchHeadCombo->count() > 0 ) {
        int idx = ui->fetchHeadCombo->currentData().toInt();
        const mybranch & b = lgit_->branches()[ idx ];
        logOfTip( QString::fromStdString( b.tip_sha ), QString::fromStdString( b.name ) );
    }

    return 0;
}

int PullDialog::logOfTip( QString sha, QString hide )
{
    lgit_->readLog( sha, hide );
    return 0;
}
