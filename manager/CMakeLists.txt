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

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules" )
include(FindPkgConfig)

# Find includes in corresponding build directories
set( CMAKE_INCLUDE_CURRENT_DIR ON )

# Run moc automatically when required
set( CMAKE_AUTOMOC ON )

#
# Qt5Widgets
#

find_package( Qt5Widgets ${QT_VERSION_REQ} QUIET )
if( NOT Qt5Widgets_FOUND )
    message( "=" )
    message( "= Could not find Qt5 (its Widgets component). Set CMAKE_PREFIX_PATH to where Qt is installed. =" )
    message( "=" )
    message( FATAL_ERROR "Some components have not been found" )
endif()

get_target_property( QT_QMAKE_EXECUTABLE Qt5::qmake LOCATION )

#
# CoreFoundation
#

if( "${CMAKE_SYSTEM_NAME}" MATCHES "Darwin" )
    find_package( CoreFoundation QUIET )
    if( NOT CoreFoundation_FOUND )
        message( "=" )
        message( "= Could not find CoreFoundation library. Set CMAKE_FRAMEWORK_PATH to where Xcode's frameworks are located. =" )
        message( "= It's a path like: /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.11.sdk/System/Library/Frameworks/ =" )
        message( "=" )
        message( FATAL_ERROR "Some components have not been found" )
    endif()
endif()

#
# Curl
#

pkg_check_modules( CURL libcurl )

if( NOT CURL_FOUND )
    message( "=" )
    message( "= Note: Curl not found =" )
    message( "=" )
endif()

#
# HTTP_Parser
#

find_package( HTTP_Parser QUIET )
if( NOT HTTP_PARSER_FOUND OR NOT HTTP_PARSER_VERSION_MAJOR EQUAL 2 )
    message( "=" )
    message( "= http-parser was not found or is too old; using bundled 3rd-party sources." )
    message( "=" )
endif()

#
# OpenSSL
#

if( NOT "${CMAKE_SYSTEM_NAME}" MATCHES "Darwin" )
    find_package( OpenSSL QUIET )

    if( NOT OPENSSL_FOUND )
        message( "=" )
        message( "= Warning: OpenSSL not found, QtZekyllManager might not build =" )
        message( "=" )
    endif()
endif()

#
# Iconv
#

find_path( ICONV_INCLUDE_DIRS iconv.h )
find_library( ICONV_LIBRARIES NAMES iconv libiconv libiconv-2 c )

if( NOT ICONV_INCLUDE_DIRS OR NOT ICONV_LIBRARIES )
    message( "=" )
    message( "= Warning: Iconv not found, QtZekyllManager might not build =" )
    message( "=" )
endif()

#
# Security
#

if( "${CMAKE_SYSTEM_NAME}" MATCHES "Darwin" )
    find_package( Security QUIET )
    if( NOT SECURITY_FOUND )
        message( "=" )
        message( "= Warning: Security framework not found, QtZekyllManager might not build =" )
        message( "= One possible path where the framework should be is: /System/Library/Frameworks/Security.framework/ =" )
        message( "=" )
    endif()
endif()

#
# LibSSH
#

pkg_check_modules( LIBSSH2 libssh2 )
if( NOT LIBSSH2_FOUND )
    message( "=" )
    message( "= LibSSH2 not found, QtZekyllManager will not build correctly =" )
    message( "=" )
endif()

#
# Zlib
#

find_package( ZLIB QUIET )
if( NOT ZLIB_FOUND )
    message( "=" )
    message( "= zlib was not found; using bundled 3rd-party sources. =" )
    message( "=" )
endif()


#
# Follow source files, headers
#

SET( SRCS
    main.cpp
    closewithoutsavingdialog.cpp
    coding_functions.cpp
    lzcsde.cpp
    lzcsde_entry.cpp
    mainwindow.cpp
    math_functions.cpp
    messages.cpp
    myqtabwidget.cpp
    script_functions.cpp
    zkiresize.cpp
    zkrewrite.cpp
    zmeditor.cpp
    myqtablewidget.cpp
    git.cpp
    lgit.cpp
    myqlineedit.cpp
    commitdialog.cpp
    lgit_branches.cpp
    pulldialog.cpp
    lgit_current.cpp
    lgit_remotes.cpp
    lgit_log.cpp
    gitoperationtracker.cpp
)

QT5_WRAP_UI( UIS_HDRS
    mainwindow.ui
    zmeditor.ui
    closewithoutsavingdialog.ui
    commitdialog.ui
    pulldialog.ui
)

# Needed -DQT_WIDGETS_LIB for Qt 5
add_definitions( ${Qt5Widgets_DEFINITIONS} )

# Executables fail to build with Qt 5 without -fPIE
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

link_directories( ${CURL_LIBRARY_DIRS} ${LIBSSH2_LIBRARY_DIRS} )

add_executable( QtZekyllManager ${SRCS} ${UIS_HDRS} )

target_link_libraries( QtZekyllManager Qt5::Widgets ${CoreFoundation_LIBRARIES}
                       ${ZLIB_LIBRARIES} ${CURL_LIBRARIES} ${HTTP_PARSER_LIBRARIES} ${OPENSSL_LIBRARIES}
                       ${ICONV_LIBRARIES} ${SECURITY_LIBRARIES} ${LIBSSH2_LIBRARIES}
                       $<TARGET_PROPERTY:git2,INTERFACE_LINK_LIBRARIES>
                       $<TARGET_PROPERTY:QScintilla2,INTERFACE_LINK_LIBRARIES>
)

target_include_directories( QtZekyllManager PRIVATE ${Qt5Widgets_INCLUDE_DIRS} ${CoreFoundation_INCLUDE_DIRS}
                            ${ZLIB_INCLUDE_DIRS} ${CURL_INCLUDE_DIRS} ${HTTP_PARSER_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR}
                            ${ICONV_INCLUDE_DIR} ${SECURITY_INCLUDE_DIRS} ${LIBSSH2_INCLUDE_DIRS}
                            $<TARGET_PROPERTY:git2,INTERFACE_INCLUDE_DIRECTORIES>
                            $<TARGET_PROPERTY:QScintilla2,INTERFACE_INCLUDE_DIRECTORIES>
)
