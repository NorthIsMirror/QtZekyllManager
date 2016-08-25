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

#include "zmeditor.h"
#include "ui_zmeditor.h"

#include "closewithoutsavingdialog.h"

#include <QFile>
#include <QColor>

#include "Qsci/qscilexerbatch.h"
#include "Qsci/qscilexermakefile.h"
#include "Qsci/qscilexertex.h"

ZMEditor::ZMEditor(QWidget *parent) :
    QDialog(parent),
    current_lexer_(0),
    ui(new Ui::ZMEditor)
{
    ui->setupUi(this);

    setAttribute( Qt::WA_DeleteOnClose );

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

    lexer_[0] = new QsciLexerBatch;
    lexer_[0]->setDefaultFont( font_ );

    lexer_[1] = new QsciLexerTeX;
    lexer_[1]->setDefaultFont( font_ );

    lexer_[2] = new QsciLexerMakefile;
    lexer_[2]->setDefaultFont( font_ );

    this->ui->textEdit->setLexer( lexer_[ current_lexer_ ] );
    this->ui->textEdit->SendScintilla( QsciScintilla::SCI_STYLESETFONT, 1, "Courier" );
}

ZMEditor::~ZMEditor()
{
    delete lexer_[2];
    delete lexer_[1];
    delete lexer_[0];
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

void ZMEditor::on_highlight_clicked()
{
    current_lexer_ = (current_lexer_ + 1) % 3;
    ui->textEdit->setLexer( lexer_[ current_lexer_ ] );
    ui->textEdit->SendScintilla( QsciScintilla::SCI_STYLESETFONT, 1, "Courier" );

    ui->highlight->setText( QString("Highlight: %1").arg( current_lexer_ + 1 ) );
}
