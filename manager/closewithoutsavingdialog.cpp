#include "closewithoutsavingdialog.h"
#include "ui_closewithoutsavingdialog.h"

CloseWithoutSavingDialog::CloseWithoutSavingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CloseWithoutSavingDialog)
{
    ui->setupUi(this);
}

CloseWithoutSavingDialog::~CloseWithoutSavingDialog()
{
    delete ui;
}

void CloseWithoutSavingDialog::on_buttonBox_accepted()
{
    emit accepted(true);
}

void CloseWithoutSavingDialog::on_buttonBox_rejected()
{
    emit accepted(false);
}
