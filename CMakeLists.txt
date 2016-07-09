cmake_minimum_required( VERSION 2.8.12 )
project( rootproject )

# Edit this path to point to your prefix of Qt installation
# directory. Prefix means: path up to and not including "bin"
set( CMAKE_PREFIX_PATH $ENV{HOME}/Qt/5.7/clang_64 )
 
add_subdirectory(manager)
