#ifndef CLOSEWITHOUTSAVINGDIALOG_H
#define CLOSEWITHOUTSAVINGDIALOG_H

#include <QDialog>

namespace Ui {
class CloseWithoutSavingDialog;
}

class CloseWithoutSavingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CloseWithoutSavingDialog(QWidget *parent = 0);
    ~CloseWithoutSavingDialog();

signals:
    void accepted(bool accepted);

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::CloseWithoutSavingDialog *ui;
};

#endif // CLOSEWITHOUTSAVINGDIALOG_H
