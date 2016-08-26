#ifndef STATUSDIALOG_H
#define STATUSDIALOG_H

#include <QString>
#include <QDialog>

namespace Ui {
class StatusDialog;
}

class StatusDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StatusDialog(QWidget *parent = 0);
    ~StatusDialog();
    void setText( const QString & richText );


private:
    Ui::StatusDialog *ui;

    QString richText_;
};

#endif // STATUSDIALOG_H
