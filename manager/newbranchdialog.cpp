#include "newbranchdialog.h"
#include "ui_newbranchdialog.h"

#include "singleton.h"
#include "messages.h"
#include "logdialog.h"

#define MessagesI Singleton<Messages>::instance()

NewBranchDialog::NewBranchDialog( QWidget *parent ) :
    QDialog( parent ),
    use_head_( false ),
    unclear_( false ),
    do_checkout_( false ),
    ui( new Ui::NewBranchDialog )
{
    ui->setupUi( this );
}

NewBranchDialog::~NewBranchDialog()
{
    delete ui;
}

void NewBranchDialog::on_buttonBox_accepted()
{
    branch_name_ = ui->branchName->text().trimmed();
    commit_sha_ = ui->commitSha->text().trimmed();

    if ( ui->workdirAsParentRadio->isChecked() ) {
        use_head_ = true;
    } else if ( ui->specifiedCommitRadio->isChecked() ){
        use_head_ = false;
    } else {
        MessagesI.AppendMessageT( "Unclear result of New Branch dialog" );
        unclear_ = true;
    }

    if ( ui->checkoutCheckBox->isChecked() ) {
        do_checkout_ = true;
    } else {
        do_checkout_ = false;
    }

    if ( branch_name_.isEmpty() ) {
        unclear_ = true;
    }

    if ( !use_head_ && commit_sha_.isEmpty() ) {
        unclear_ = true;
    }
}

void NewBranchDialog::on_pickButton_clicked()
{
    int error;

    if ( !ui->specifiedCommitRadio->isChecked() ) {
        ui->specifiedCommitRadio->setChecked( true );
    }

    LogDialog *ldialog = new LogDialog( this );
    ldialog->setLGit( lgit_ );
    if ( ( error = ldialog->reset() ) < 0 ) {
        MessagesI.AppendMessageT( QString( "Could not pick commit, exit code: %1" ).arg( error ) );
    } else {
        if ( ldialog->exec() == QDialog::Accepted ) {
            ui->commitSha->setText( ldialog->selectedSha() );
        }
        ldialog->deleteLater();
    }
}
