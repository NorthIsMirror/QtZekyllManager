#ifndef EDITOR_H
#define EDITOR_H

#include <QDialog>
#include <QString>

namespace Ui {
class Editor;
}

class Editor : public QDialog
{
    Q_OBJECT

public:
    explicit Editor(QWidget *parent = 0);
    ~Editor();

    void setFile( QString fullPath );

public slots:
    void closeWithoutSaving(bool _close);

private slots:
    void on_save_clicked();

    void on_close_clicked();

    void on_cancel_clicked();

private:
    Ui::Editor *ui;
    QString filePath_;
};

#endif // EDITOR_H
