#ifndef DELETEBRANCHDIALOG_H
#define DELETEBRANCHDIALOG_H

#include <QString>
#include <QDialog>

namespace Ui {
class DeleteBranchTagDialog;
}

class DeleteBranchTagDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DeleteBranchTagDialog( QWidget *parent = 0 );
    ~DeleteBranchTagDialog();
    int addBranch( const QString & name, const QString & tip_sha );
    int addTag( const QString & name, const QString & tip_sha );
    int endAdding();
    const QString & selectedRef() const { return selectedRef_; }

private slots:
    void on_buttonBox_accepted();

private:

    QString selectedRef_;

    Ui::DeleteBranchTagDialog *ui;
};

#endif // DELETEBRANCHDIALOG_H
