/*
 Copyright 2016 Sebastian Gniazdowski

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
