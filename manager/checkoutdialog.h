#ifndef CHECKOUTDIALOG_H
#define CHECKOUTDIALOG_H

#include <QDialog>

namespace Ui {
class CheckoutDialog;
}

class CheckoutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CheckoutDialog(QWidget *parent = 0);
    ~CheckoutDialog();

    int addBranch( const QString & name, const QString & sha_tip );
    int addTag( const QString & name, const QString & sha_tip );

private:
    int addNonselectable( const QString & title );
    int startBranches();
    int startTags();

    bool inserting_branches_;
    bool inserting_tags_;

    Ui::CheckoutDialog *ui;
};

#endif // CHECKOUTDIALOG_H
