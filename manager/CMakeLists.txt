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
if( APPLE )
    find_package( Qt5MacExtras ${QT_VERSION_REQ} REQUIRED )
endif()
get_target_property( QT_QMAKE_EXECUTABLE Qt5::qmake LOCATION )

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
    zkrewrite.cpp
)

QT5_WRAP_UI( UIS_HDRS
    mainwindow.ui
    zmeditor.ui
    closewithoutsavingdialog.ui
)

# Also includes directories for dependencies QtCore and QtGui
include_directories( ${Qt5Widgets_INCLUDE_DIRS} )

# Needed -DQT_WIDGETS_LIB for Qt 5
add_definitions( ${Qt5Widgets_DEFINITIONS} ${Qt5MacExtras_DEFINITIONS} )

# Executables fail to build with Qt 5 without -fPIE
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

add_executable( QtZekyllManager ${SRCS} ${UIS_HDRS} )

if( APPLE )
    target_link_libraries( QtZekyllManager Qt5::Widgets Qt5::MacExtras
                           $<TARGET_PROPERTY:QScintilla2,INTERFACE_LINK_LIBRARIES>
)
else()
    target_link_libraries( QtZekyllManager Qt5::Widgets
                           $<TARGET_PROPERTY:QScintilla2,INTERFACE_LINK_LIBRARIES>
)
endif()

target_include_directories( QtZekyllManager PRIVATE
                            $<TARGET_PROPERTY:QScintilla2,INTERFACE_INCLUDE_DIRECTORIES>
)
