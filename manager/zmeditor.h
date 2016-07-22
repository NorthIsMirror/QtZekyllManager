#ifndef EDITOR_H
#define EDITOR_H

#include <QDialog>
#include <QString>
#include <QFont>
#include "Qsci/qsciscintilla.h"
#include "Qsci/qscilexer.h"

namespace Ui {
class ZMEditor;
}

class ZMEditor : public QDialog
{
    Q_OBJECT

public:
    explicit ZMEditor(QWidget *parent = 0);
    ~ZMEditor();

    void setFile( QString fullPath );

public slots:
    void closeWithoutSaving(bool _close);

private slots:
    void on_save_clicked();

    void on_close_clicked();

    void on_cancel_clicked();

private:
    Ui::ZMEditor *ui;
    QString filePath_;

    QFont font_;
    QsciLexer *lexer_;
};

#endif // EDITOR_H
