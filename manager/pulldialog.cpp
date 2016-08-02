#include "pulldialog.h"
#include "ui_pulldialog.h"

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

    if( lgit_->current().type() == CURRENT_TYPE_BRANCH ) {
        ui->checkoutLabel->setText( tr("Currently checked out: ") + QString::fromStdString( lgit_->current().branch() ) + tr(" (branch)") );
    } else if ( lgit_->current().type() == CURRENT_TYPE_TAG ) {
        ui->checkoutLabel->setText( tr("Currently checked out: ") + QString::fromStdString( lgit_->current().tag() ) + tr(" (tag)") );
    } else if ( lgit_->current().type() == CURRENT_TYPE_OID ) {
        ui->checkoutLabel->setText( tr("Detached HEAD points to: ") + QString::fromStdString( lgit_->current().oid() ) );
    }
}
