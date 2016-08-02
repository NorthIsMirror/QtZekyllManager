#include "pulldialog.h"
#include "ui_pulldialog.h"

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
}
