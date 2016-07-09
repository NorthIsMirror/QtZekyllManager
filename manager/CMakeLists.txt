cmake_minimum_required( VERSION 2.8.12 )
project( QtZekyllManager )
set( QT_VERSION_REQ "5.2" )
set( CMAKE_CXX_STANDARD 11 )

if( NOT CMAKE_PREFIX_PATH )
    # Edit this path to point to your prefix of Qt installation
    # directory. Prefix means: path up to and not including "bin"
    set( CMAKE_PREFIX_PATH $ENV{HOME}/Qt/5.7/clang_64 )
    #set( CMAKE_PREFIX_PATH /usr/local/Cellar/qt5/5.6.1 )
endif()

# Find includes in corresponding build directories
set( CMAKE_INCLUDE_CURRENT_DIR ON )

# Run moc automatically when required
set( CMAKE_AUTOMOC ON )

# Widgets finds its own dependencies (QtGui and QtCore).
find_package( Qt5Widgets ${QT_VERSION_REQ} REQUIRED )

SET( SRCS
    closewithoutsavingdialog.cpp
    coding_functions.cpp
    zmeditor.cpp
    lzcsde.cpp
    lzcsde_entry.cpp
    main.cpp
    mainwindow.cpp
    math_functions.cpp
    messages.cpp
    myqtabwidget.cpp
    script_functions.cpp
    zkiresize.cpp
)

QT5_WRAP_UI( UIS_HDRS
    mainwindow.ui
    zmeditor.ui
    closewithoutsavingdialog.ui
)

# Also includes directories for dependencies QtCore and QtGui
include_directories( ${Qt5Widgets_INCLUDE_DIRS} )

# Needed -DQT_WIDGETS_LIB for Qt 5
add_definitions( ${Qt5Widgets_DEFINITIONS} )

# Executables fail to build with Qt 5 without -fPIE
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

add_executable( QtZekyllManager ${SRCS} ${UIS_HDRS} )

target_link_libraries( QtZekyllManager Qt5::Widgets )

target_include_directories( QtZekyllManager PRIVATE
    $<TARGET_PROPERTY:QScintilla2,INTERFACE_INCLUDE_DIRECTORIES> )
