#include "zmeditor.h"
#include "ui_zmeditor.h"
#include "closewithoutsavingdialog.h"
#include <QFile>

ZMEditor::ZMEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ZMEditor)
{
    ui->setupUi(this);
}

ZMEditor::~ZMEditor()
{
    delete ui;
}

void ZMEditor::setFile(QString fullPath) {
    filePath_ = fullPath;
    QFile file( filePath_ );
    file.open(QFile::ReadOnly | QFile::Text);
    //ui->textEdit->setText(file.readAll());
}

void ZMEditor::on_save_clicked()
{
    QFile file( filePath_ );
    file.open(QFile::WriteOnly | QFile::Text | QFile::Truncate);
    //file.write( ui->textEdit->text().toUtf8().constData() );
    //ui->textEdit->setModified(false);
}


void ZMEditor::on_close_clicked()
{
    if( ui->textEdit->document()->isModified() ) {
        CloseWithoutSavingDialog *dialog = new CloseWithoutSavingDialog(this);
        QObject::connect( dialog, SIGNAL(accepted(bool)), this, SLOT(closeWithoutSaving(bool)) );

        dialog->exec();
    } else {
        close();
    }
}

void ZMEditor::closeWithoutSaving(bool _close) {
    if(_close) {
        close();
    }
}

void ZMEditor::on_cancel_clicked()
{
    close();
}
