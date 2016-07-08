#include "editor.h"
#include "ui_editor.h"
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
    ui->textArea->setPlainText(file.readAll());
}

void Editor::on_save_clicked()
{
    QFile file( filePath_ );
    file.open(QFile::WriteOnly | QFile::Text | QFile::Truncate);
    file.write( ui->textArea->toPlainText().toUtf8().constData() );
}
