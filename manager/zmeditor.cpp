#include "zmeditor.h"
#include "ui_zmeditor.h"
#include "closewithoutsavingdialog.h"
#include <QFile>
#include <QColor>
#include "Qsci/qscilexerbatch.h"

ZMEditor::ZMEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ZMEditor)
{
    ui->setupUi(this);


    font_.setFamily( "Courier" );
    font_.setFixedPitch( true );
    font_.setPointSize( 15 );
    ui->textEdit->setFont( font_ );
    ui->textEdit->setMarginsFont( font_ );

    QFontMetrics fontmetrics( font_ );
    ui->textEdit->setMarginsFont( font_ );
    ui->textEdit->setMarginWidth( 0, fontmetrics.width("0000") + 6 );
    ui->textEdit->setMarginLineNumbers( 0, true );
    ui->textEdit->setMarginsBackgroundColor( QColor("#cccccc") );

    //ui->textEdit->setBraceMatching( QsciScintilla::SloppyBraceMatch );

    ui->textEdit->setCaretLineVisible( true );
    ui->textEdit->setCaretLineBackgroundColor( QColor( "#ffe4e4" ) );

    lexer_ = new QsciLexerBatch;
    lexer_->setDefaultFont( font_ );
    this->ui->textEdit->setLexer( lexer_ );
    this->ui->textEdit->SendScintilla( QsciScintilla::SCI_STYLESETFONT, 1, "Courier" );
}

ZMEditor::~ZMEditor()
{
    delete ui;
}

void ZMEditor::setFile(QString fullPath) {
    filePath_ = fullPath;
    QFile file( filePath_ );
    file.open(QFile::ReadOnly | QFile::Text);
    ui->textEdit->setText(file.readAll());
    ui->textEdit->setModified(false);
}

void ZMEditor::on_save_clicked()
{
    QFile file( filePath_ );
    file.open(QFile::WriteOnly | QFile::Text | QFile::Truncate);
    file.write( ui->textEdit->text().toUtf8().constData() );
    ui->textEdit->setModified(false);
}


void ZMEditor::on_close_clicked()
{
    if( ui->textEdit->isModified() ) {
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
