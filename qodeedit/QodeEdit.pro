TEMPLATE = lib
CONFIG += staticlib

include( "./autogen.pri" )

lessThan(QT_MAJOR_VERSION, 5): error("QtZekyllManager (and current component that you build: QodeEdit) requires Qt 5")

ABSOLUTE_DATA_DIR = $$PWD/share
VERSION = 0.1.0
autoGenerateFile( "QodeEditConfig.h.in", "src/QodeEditConfig.h" )

QT += xml
QT += widgets

INCLUDEPATH += $${PWD}/src

HEADERS +=  \
    src/QodeEdit.h \
    src/Tools.h \
    src/editor/CodeEditor.h \
    src/editor/TextDocument.h \
    src/editor/PlainTextDocumentLayout.h \
    src/editor/TextBlockUserData.h \
    src/margin/MarginStacker.h \
    src/margin/AbstractMargin.h \
    src/margin/LineBookmarkMargin.h \
    src/margin/LineNumberMargin.h \
    src/margin/LineRevisionMargin.h \
    src/margin/LineSpacingMargin.h \
    src/syntax/Helpers.h \
    src/syntax/List.h \
    src/syntax/Document.h \
    src/syntax/ItemData.h \
    src/syntax/Rule.h \
    src/syntax/Comment.h \
    src/syntax/Keywords.h \
    src/syntax/Highlighting.h \
    src/syntax/Context.h \
    src/syntax/General.h \
    src/syntax/Folding.h \
    src/syntax/EmptyLine.h \
    src/syntax/Indentation.h \
    src/syntax/SpellChecking.h \
    src/syntax/Configuration.h \
    src/syntax/Encoding.h \
    src/syntax/Parser.h \
    src/syntax/DocumentBuilder.h \
    src/syntax/Model.h \
    src/syntax/Highlighter.h \
    src/syntax/ComboBox.h \
    src/syntax/ContextStack.h \
    src/syntax/ContextSwitcher.h \
    src/theme/Theme.h \
    src/Threading.h \
    src/Manager.h

SOURCES +=  \
    src/QodeEdit.cpp \
    src/Tools.cpp \
    src/editor/CodeEditor.cpp \
    src/editor/TextDocument.cpp \
    src/editor/PlainTextDocumentLayout.cpp \
    src/editor/TextBlockUserData.cpp \
    src/margin/MarginStacker.cpp \
    src/margin/AbstractMargin.cpp \
    src/margin/LineBookmarkMargin.cpp \
    src/margin/LineNumberMargin.cpp \
    src/margin/LineRevisionMargin.cpp \
    src/margin/LineSpacingMargin.cpp \
    src/syntax/Document.cpp \
    src/syntax/ItemData.cpp \
    src/syntax/Rule.cpp \
    src/syntax/Comment.cpp \
    src/syntax/Keywords.cpp \
    src/syntax/Highlighting.cpp \
    src/syntax/Context.cpp \
    src/syntax/General.cpp \
    src/syntax/Folding.cpp \
    src/syntax/EmptyLine.cpp \
    src/syntax/Indentation.cpp \
    src/syntax/SpellChecking.cpp \
    src/syntax/Configuration.cpp \
    src/syntax/Encoding.cpp \
    src/syntax/Parser.cpp \
    src/syntax/DocumentBuilder.cpp \
    src/syntax/Model.cpp \
    src/syntax/Highlighter.cpp \
    src/syntax/ComboBox.cpp \
    src/syntax/ContextStack.cpp \
    src/syntax/ContextSwitcher.cpp \
    src/theme/Theme.cpp \
    src/Threading.cpp \
    src/Manager.cpp
