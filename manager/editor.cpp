#include "editor.h"
#include "ui_editor.h"
#include "closewithoutsavingdialog.h"
#include <QFile>

Editor::Editor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Editor)
{
    ui->setupUi(this);
}

Editor::~Editor()
{
    delete ui;
}

void Editor::setFile(QString fullPath) {
    filePath_ = fullPath;
    QFile file( filePath_ );
    file.open(QFile::ReadOnly | QFile::Text);
    ui->textEdit->setText(file.readAll());
}

void Editor::on_save_clicked()
{
    QFile file( filePath_ );
    file.open(QFile::WriteOnly | QFile::Text | QFile::Truncate);
    file.write( ui->textEdit->text().toUtf8().constData() );
    ui->textEdit->setModified(false);
}


void Editor::on_close_clicked()
{
    if( ui->textEdit->isModified() ) {
        CloseWithoutSavingDialog *dialog = new CloseWithoutSavingDialog(this);
        QObject::connect( dialog, SIGNAL(accepted(bool)), this, SLOT(closeWithoutSaving(bool)) );

        dialog->exec();
    } else {
        close();
    }
}

void Editor::closeWithoutSaving(bool _close) {
    if(_close) {
        close();
    }
}

void Editor::on_cancel_clicked()
{
    close();
}
