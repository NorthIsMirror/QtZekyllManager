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

    lexer_ = new QsciLexerMakefile;
    lexer_->setDefaultFont( font_ );

    this->ui->textEdit->setLexer( lexer_ );
    this->ui->textEdit->SendScintilla( QsciScintilla::SCI_STYLESETFONT, 1, "Courier" );
}

CommitDialog::~CommitDialog()
{
    delete ui;
}

void CommitDialog::on_buttonBox_accepted()
{
    accepted_ = true;
    commitMessage_ = ui->textEdit->text();
}

void CommitDialog::on_buttonBox_rejected()
{
    accepted_ = false;
    commitMessage_ = "";
}
