#include "deletebranchtagdialog.h"
#include "ui_deletebranchtagdialog.h"

#include <QDebug>

struct MyRefData {
    QString name;
    QString tip_sha;
    bool is_branch;
    bool is_tag;

    MyRefData() : is_branch( false ), is_tag( false )
    { }

    MyRefData( const QString & _name, const QString & _tip_sha, bool _is_branch, bool _is_tag ) :
        name( _name ), tip_sha( _tip_sha ), is_branch( _is_branch ), is_tag( _is_tag )
    { }
};

Q_DECLARE_METATYPE( MyRefData );

DeleteBranchTagDialog::DeleteBranchTagDialog( QWidget *parent ) :
    QDialog( parent ),
    ui( new Ui::DeleteBranchTagDialog )
{
    ui->setupUi(this);
}

DeleteBranchTagDialog::~DeleteBranchTagDialog()
{
    delete ui;
}

int DeleteBranchTagDialog::addBranch( const QString & name, const QString & tip_sha )
{
    MyRefData refData( name, tip_sha, true, false );
    QVariant vRefData = QVariant::fromValue( refData );

    ui->comboBox->addItem( name, vRefData );

    return 0;
}

int DeleteBranchTagDialog::addTag(const QString &name, const QString &tip_sha)
{
    MyRefData refData( name, tip_sha, false, true );
    QVariant vRefData = QVariant::fromValue( refData );

    ui->comboBox->addItem( name, vRefData );

    return 0;
}

int DeleteBranchTagDialog::endAdding()
{
    if ( ui->comboBox->count() <= 0 ) {
        MyRefData refData( "", "", false, false );
        QVariant vRefData = QVariant::fromValue( refData );

        ui->comboBox->addItem( tr( "No branches in repository" ), vRefData );
    }

    return 0;
}

void DeleteBranchTagDialog::on_buttonBox_accepted()
{
    selectedRef_ = ui->comboBox->currentText();
}
