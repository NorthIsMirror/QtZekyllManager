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

#include "commitdialog.h"
#include "ui_commitdialog.h"
#include "Qsci/qsciscintilla.h"
#include "Qsci/qscilexermakefile.h"

CommitDialog::CommitDialog(QWidget *parent) :
    QDialog(parent),
    accepted_(false),
    ui(new Ui::CommitDialog)
{
    ui->setupUi(this);

    font_.setFamily( "Courier" );
    font_.setFixedPitch( true );
    font_.setPointSize( 15 );
    ui->textEdit->setFont( font_ );
    ui->textEdit->setMarginsFont( font_ );

    QFontMetrics fontmetrics( font_ );
    ui->textEdit->setMarginsFont( font_ );
    ui->textEdit->setMarginWidth( 0, fontmetrics.width("000") + 6 );
    ui->textEdit->setMarginLineNumbers( 0, true );
    ui->textEdit->setMarginsBackgroundColor( QColor("#cccccc") );

    ui->textEdit->setBraceMatching( QsciScintilla::SloppyBraceMatch );

    // ui->textEdit->setCaretLineVisible( true );
    // ui->textEdit->setCaretLineBackgroundColor( QColor( "#ffe4e4" ) );

    ui->textEdit->setWrapMode( QsciScintilla::WrapWord );
    ui->textEdit->setEdgeColor( QColor("#ee5555") );
    ui->textEdit->setEdgeColumn( 72 );
    ui->textEdit->setEdgeMode( QsciScintilla::EdgeBackground );

    // lexer_ = new QsciLexerMakefile;
    // lexer_->setDefaultFont( font_ );

    // this->ui->textEdit->setLexer( lexer_ );
    this->ui->textEdit->SendScintilla( QsciScintilla::SCI_STYLESETFONT, 1, "Courier" );
}

CommitDialog::~CommitDialog()
{
    delete ui;
}

void CommitDialog::on_buttonBox_accepted()
{
    accepted_ = true;
    commitMessage_ = ui->textEdit->text().trimmed();
    name_ = ui->name->text().trimmed();
    email_ = ui->email->text().trimmed();
}

void CommitDialog::on_buttonBox_rejected()
{
    accepted_ = false;
    commitMessage_ = "";
}

void CommitDialog::setCommitMessage( const QString & msg ) {
    commitMessage_ = msg.trimmed();
    ui->textEdit->setText( commitMessage_ );
}

void CommitDialog::setEmail( const QString & email ) {
    email_ = email.trimmed();
    ui->email->setText( email_ );
}

void CommitDialog::setName( const QString & name ) {
    name_ = name.trimmed();
    ui->name->setText( name_ );
}

bool CommitDialog::addParent( const std::string & parent )
{
    return addParent( QString::fromStdString( parent ) );
}

bool CommitDialog::addParent( const QString & parent )
{
    if( parents_.indexOf( parent ) != -1 ) {
        return false;
    }

    parents_.append( parent );

    return true;
}
