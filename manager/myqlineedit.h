#ifndef MYQLINEEDIT_H
#define MYQLINEEDIT_H

#include <QObject>
#include <QLineEdit>

class MyQLineEdit : public QLineEdit
{
    Q_OBJECT

protected:
    void focusInEvent( QFocusEvent* e );

public:
    MyQLineEdit( QWidget *parent = 0 );
};

#endif // MYQLINEEDIT_H
