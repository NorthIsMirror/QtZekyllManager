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
    zmeditor.cpp \
    myqtablewidget.cpp \
    git.cpp \
    lgit.cpp

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
    zkiresize.h \
    zkrewrite.h \
    zmeditor.h \
    myqtablewidget.h \
    git.h \
    lgit.h

FORMS    += mainwindow.ui \
    zmeditor.ui \
    closewithoutsavingdialog.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../libgit2/release/ -lgit2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../libgit2/debug/ -lgit2
else:unix: LIBS += -L$$PWD/../libgit2/ -lgit2

INCLUDEPATH += $$PWD/../libgit2/include
DEPENDPATH += $$PWD/../libgit2/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../libgit2/release/libgit2.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../libgit2/debug/libgit2.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../libgit2/release/git2.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../libgit2/debug/git2.lib
else:unix: PRE_TARGETDEPS += $$PWD/../libgit2/libgit2.a
