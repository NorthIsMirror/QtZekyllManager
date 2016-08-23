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

    void on_highlight_clicked();

private:
    QString filePath_;

    QFont font_;
    QsciLexer *lexer_[3];
    int current_lexer_;

    Ui::ZMEditor *ui;
};

#endif // EDITOR_H
