#-------------------------------------------------
#
# Project created by QtCreator 2016-07-05T17:15:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets macextras

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
    lgit.cpp \
    myqlineedit.cpp \
    commitdialog.cpp \
    lgit_branches.cpp \
    pulldialog.cpp \
    lgit_current.cpp \
    lgit_remotes.cpp \
    lgit_log.cpp \
    gitoperationtracker.cpp \
    external_functions.cpp \
    checkoutdialog.cpp \
    lgit_tags.cpp \
    progresswidget.cpp \
    lgit_status.cpp

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
    lgit.h \
    myqlineedit.h \
    selectedzekylls.h \
    commitdialog.h \
    lgit_branches.h \
    pulldialog.h \
    lgit_current.h \
    lgit_remotes.h \
    lgit_log.h \
    gitoperationtracker.h \
    external_functions.h \
    checkoutdialog.h \
    lgit_tags.h \
    progresswidget.h \
    lgit_status.h

FORMS    += mainwindow.ui \
    zmeditor.ui \
    closewithoutsavingdialog.ui \
    commitdialog.ui \
    pulldialog.ui \
    checkoutdialog.ui \
    progresswidget.ui

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



MAC_SDK  = /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.11.sdk
if( !exists( $$MAC_SDK) ) {
  error("The selected Mac OSX SDK does not exist at $$MAC_SDK!")
}

# CoreFoundation
QMAKE_LFLAGS += -F$$MAC_SDK/System/Library/Frameworks/CoreFoundation.framework
LIBS += -framework CoreFoundation

# Security
QMAKE_LFLAGS += -F/System/Library/Frameworks/Security.framework
LIBS += -framework Security

# Iconv
macx: LIBS += -L$$PWD/../../../../../usr/lib/ -liconv
INCLUDEPATH += $$PWD/../../../../../Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.11.sdk/usr/include
DEPENDPATH += $$PWD/../../../../../Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.11.sdk/usr/include

# Curl
macx: LIBS += -L$$PWD/../../../../../usr/lib/ -lcurl

INCLUDEPATH += $$PWD/../../../../../Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.11.sdk/usr/include
DEPENDPATH += $$PWD/../../../../../Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.11.sdk/usr/include

# LibSSH2
macx: LIBS += -L$$PWD/../../../../../usr/local/Cellar/libssh2/1.7.0/lib/ -lssh2

INCLUDEPATH += $$PWD/../../../../../usr/local/Cellar/libssh2/1.7.0/include
DEPENDPATH += $$PWD/../../../../../usr/local/Cellar/libssh2/1.7.0/include

# Zlib
macx: LIBS += -L$$PWD/../../../../../usr/lib/ -lz

INCLUDEPATH += $$PWD/../../../../../Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.11.sdk/usr/include
DEPENDPATH += $$PWD/../../../../../Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.11.sdk/usr/include

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../qscintilla2/Qt4Qt5/release/ -lqscintilla2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../qscintilla2/Qt4Qt5/debug/ -lqscintilla2
else:unix: LIBS += -L$$OUT_PWD/../qscintilla2/Qt4Qt5/ -lqscintilla2

INCLUDEPATH += $$PWD/../qscintilla2/Qt4Qt5
DEPENDPATH += $$PWD/../qscintilla2/Qt4Qt5

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../qscintilla2/Qt4Qt5/release/libqscintilla2.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../qscintilla2/Qt4Qt5/debug/libqscintilla2.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../qscintilla2/Qt4Qt5/release/qscintilla2.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../qscintilla2/Qt4Qt5/debug/qscintilla2.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../qscintilla2/Qt4Qt5/libqscintilla2.a
