#ifndef MYCODEEDITOR_H
#define MYCODEEDITOR_H

#include "editor/CodeEditor.h"
#include <QObject>


class MyCodeEditor : public CodeEditor
{
    Q_OBJECT

public:
    MyCodeEditor( QWidget* parent = 0 );
    virtual ~MyCodeEditor();
};

#endif // MYCODEEDITOR_H
