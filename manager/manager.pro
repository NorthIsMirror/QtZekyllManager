#-------------------------------------------------
#
# Project created by QtCreator 2016-07-05T17:15:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtZekyllManager
TEMPLATE = app
CONFIG += c++11

SOURCES += main.cpp \
    closewithoutsavingdialog.cpp \
    coding_functions.cpp \
    lzcsde.cpp \
    lzcsde_entry.cpp \
    mainwindow.cpp \
    math_functions.cpp \
    messages.cpp \
    myqtabwidget.cpp \
    script_functions.cpp \
    zkiresize.cpp \
    zkrewrite.cpp \
    zmeditor.cpp

HEADERS  += mainwindow.h \
    call_once.h \
    closewithoutsavingdialog.h \
    coding_functions.h \
    lzcsde.h \
    lzcsde_entry.h \
    math_functions.h \
    messages.h \
    myqtabwidget.h \
    script_functions.h \
    singleton.h \
    ui_closewithoutsavingdialog.h \
    ui_mainwindow.h \
    ui_zmeditor.h \
    zkiresize.h \
    zkrewrite.h \
    zmeditor.h

FORMS    += mainwindow.ui \
    zmeditor.ui \
    closewithoutsavingdialog.ui
