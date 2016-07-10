cmake_minimum_required( VERSION 2.8.12 )
project( rootproject )

if( NOT CMAKE_PREFIX_PATH )
    # Edit this path to point to your prefix of Qt installation
    # directory. Prefix means: path up to and not including "bin"
    set( CMAKE_PREFIX_PATH $ENV{HOME}/Qt/5.7/clang_64 )
    #set( CMAKE_PREFIX_PATH /usr/local/Cellar/qt5/5.6.1 )
endif()
 
add_subdirectory(libgit2)
add_subdirectory(qscintilla/Qt4Qt5)
add_subdirectory(manager)
