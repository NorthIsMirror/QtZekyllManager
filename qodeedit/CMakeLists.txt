cmake_minimum_required( VERSION 2.8.12 )
project( QodeEdit )
set ( ${PROJECT_NAME}_NAME ${PROJECT_NAME} )
set ( ${PROJECT_NAME}_VERSION 0.1.0 )
set ( ${PROJECT_NAME}_ABSOLUTE_DATA_DIR ${PROJECT_SOURCE_DIR}/share )
configure_file (
    "${PROJECT_SOURCE_DIR}/QodeEditConfig_CMake.h.in"
    "${PROJECT_BINARY_DIR}/QodeEditConfig.h"
)
option( STATICLIB "Build static library (default ON)" ON )

set( QT_VERSION_REQ "5.2" )

if( NOT CMAKE_PREFIX_PATH )
    # Edit this path to point to your prefix of Qt installation
    # directory. Prefix means: path up to and not including "bin"
    set( CMAKE_PREFIX_PATH $ENV{HOME}/Qt/5.7/clang_64 )
    #set( CMAKE_PREFIX_PATH /usr/local/Cellar/qt5/5.6.1 )
endif()

function( QUERY_QMAKE VAR RESULT )
    get_target_property( QMAKE_EXEC Qt5::qmake LOCATION )
    exec_program( "${QMAKE_EXEC}" ARGS "-query ${VAR}" RETURN_VALUE retval OUTPUT_VARIABLE out )
    if( retval EQUAL 0 )
        file( TO_CMAKE_PATH "${out}" out )
        set( ${RESULT} ${out} PARENT_SCOPE )
    endif()
endfunction()

SET( HDRS
    src/QodeEdit.h
    src/Tools.h
    src/editor/CodeEditor.h
    src/editor/TextDocument.h
    src/editor/PlainTextDocumentLayout.h
    src/editor/TextBlockUserData.h
    src/margin/MarginStacker.h
    src/margin/AbstractMargin.h
    src/margin/LineBookmarkMargin.h
    src/margin/LineNumberMargin.h
    src/margin/LineRevisionMargin.h
    src/margin/LineSpacingMargin.h
    src/syntax/Helpers.h
    src/syntax/List.h
    src/syntax/Document.h
    src/syntax/ItemData.h
    src/syntax/Rule.h
    src/syntax/Comment.h
    src/syntax/Keywords.h
    src/syntax/Highlighting.h
    src/syntax/Context.h
    src/syntax/General.h
    src/syntax/Folding.h
    src/syntax/EmptyLine.h
    src/syntax/Indentation.h
    src/syntax/SpellChecking.h
    src/syntax/Configuration.h
    src/syntax/Encoding.h
    src/syntax/Parser.h
    src/syntax/DocumentBuilder.h
    src/syntax/Model.h
    src/syntax/Highlighter.h
    src/syntax/ComboBox.h
    src/syntax/ContextStack.h
    src/syntax/ContextSwitcher.h
    src/theme/Theme.h
    src/Threading.h
    src/Manager.h
)

SET( SRCS
    src/QodeEdit.cpp
    src/Tools.cpp
    src/editor/CodeEditor.cpp
    src/editor/TextDocument.cpp
    src/editor/PlainTextDocumentLayout.cpp
    src/editor/TextBlockUserData.cpp
    src/margin/MarginStacker.cpp
    src/margin/AbstractMargin.cpp
    src/margin/LineBookmarkMargin.cpp
    src/margin/LineNumberMargin.cpp
    src/margin/LineRevisionMargin.cpp
    src/margin/LineSpacingMargin.cpp
    src/syntax/Document.cpp
    src/syntax/ItemData.cpp
    src/syntax/Rule.cpp
    src/syntax/Comment.cpp
    src/syntax/Keywords.cpp
    src/syntax/Highlighting.cpp
    src/syntax/Context.cpp
    src/syntax/General.cpp
    src/syntax/Folding.cpp
    src/syntax/EmptyLine.cpp
    src/syntax/Indentation.cpp
    src/syntax/SpellChecking.cpp
    src/syntax/Configuration.cpp
    src/syntax/Encoding.cpp
    src/syntax/Parser.cpp
    src/syntax/DocumentBuilder.cpp
    src/syntax/Model.cpp
    src/syntax/Highlighter.cpp
    src/syntax/ComboBox.cpp
    src/syntax/ContextStack.cpp
    src/syntax/ContextSwitcher.cpp
    src/theme/Theme.cpp
    src/Threading.cpp
    src/Manager.cpp
)

# Find includes in corresponding build directories
set( CMAKE_INCLUDE_CURRENT_DIR ON )

# Run moc automatically when required
set( CMAKE_AUTOMOC ON )

# Widgets finds its own dependencies (QtGui and QtCore).
find_package( Qt5Widgets ${QT_VERSION_REQ} REQUIRED )
find_package( Qt5Xml ${QT_VERSION_REQ} REQUIRED )
get_target_property( QT_QMAKE_EXECUTABLE Qt5::qmake LOCATION )

QUERY_QMAKE(QT_INSTALL_BINS QT_INSTALL_BINS)
message( "" )
message( "= QT is installed at: ${_qt5Widgets_install_prefix} =" )
message( "= Qmake reports following /bin directory: ${QT_INSTALL_BINS} =" )

if( STATICLIB )
    message( "= Static build =" )
else()
    message( "= Dynamic build =" )
endif()
message( "" )

# Qt5Widgets also includes directories for dependencies QtCore and QtGui
include_directories( ${Qt5Widgets_INCLUDE_DIRS} ${Qt5Xml_INCLUDE_DIRS} src )

# Needed -DQT_WIDGETS_LIB for Qt5, and others
add_definitions( ${DEFINES} ${Qt5Widgets_DEFINITIONS} ${Qt5Xml_DEFINITIONS} )

# Executables fail to build with Qt5 without -fPIE
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

if ( STATICLIB )
    add_library( ${PROJECT_NAME} STATIC ${SRCS} ${HDRS} )
else()
    add_library( ${PROJECT_NAME} SHARED ${SRCS} ${HDRS} )
endif()

target_link_libraries( ${PROJECT_NAME} PRIVATE Qt5::Widgets Qt5::Xml )

set_property( TARGET ${PROJECT_NAME} PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}" )
set_property( TARGET ${PROJECT_NAME} PROPERTY INTERFACE_LINK_LIBRARIES "${PROJECT_BINARY_DIR}/bin/libQodeEdit.a" )
