#include "myqlineedit.h"
#include "mainwindow.h"
#include <QDebug>
#include <QApplication>
#include <tuple>

MyQLineEdit::MyQLineEdit( QWidget *parent ) : QLineEdit(parent)
{
}

void MyQLineEdit::focusInEvent(QFocusEvent *e)
{
    foreach(QWidget *widget, QApplication::topLevelWidgets()) {
      if( widget->objectName() == "MainWindow" ) {
          MainWindow *window = qobject_cast< MainWindow* >( widget );
          if( window ) {
              bool has_correct_data;
              QString input, code;
              int index;
              std::tie( has_correct_data, input, index, code ) = window->getProcessedZcodeInput();
              setText( input );

              break;
          }
      }
    }

    QLineEdit::focusInEvent(e);
}
