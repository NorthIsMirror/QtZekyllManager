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
              int has_correct_data;
              QString input;
              std::tie( has_correct_data, input ) = window->getProcessedZcodeInput();
              setText( input );

              break;
          }
      }
    }

    QLineEdit::focusInEvent(e);
}
