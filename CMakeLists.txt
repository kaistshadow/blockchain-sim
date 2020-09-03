if (POLICY CMP0048)
    cmake_policy(SET CMP0048 OLD)
endif(POLICY CMP0048)

project(BLEEP)

## ensure cmake version
cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
## ensure unix environment (CMAKE_SYSTEM_NAME == "Linux")
if((NOT UNIX) OR (NOT (CMAKE_SYSTEM_NAME STREQUAL "Linux")))
    message(FATAL_ERROR "BLEEP requires a Unix/Linux environment.")
endif((NOT UNIX) OR (NOT (CMAKE_SYSTEM_NAME STREQUAL "Linux")))

## ensure out-of-source build
message(STATUS "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message(STATUS "CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "Shadow requires an out-of-source build. Please create a separate build directory and run 'cmake path/to/shadow [options]' there.")
endif(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})

set(CMAKE_ROOT_SOURCE_DIR ${CMAKE_SOURCE_DIR})
set(CMAKE_ROOT_BINARY_DIR ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

# debug option
option(BLEEP_DEBUG "turn on debugging for BLEEP (default: ON)" ON)
option(SHADOW_DEBUG "turn on debugging for verbose program output (default: ON for BLEEP build)" ON)
# enable shadow debug as default. (Because some tests failed with release version of shadow (e.g. shadow-bitcoin-with-wallet)


## add shadow project
# should not modify CMAKE_SOURCE_DIR, CMAKE_BINARY_DIR (https://stackoverflow.com/a/22220680)
# set(CMAKE_SOURCE_DIR "${CMAKE_ROOT_SOURCE_DIR}/shadow")
# set(CMAKE_BINARY_DIR "${CMAKE_ROOT_BINARY_DIR}/shadow")

## use ExternalProject_add to use different INSTALL_PREFIX (https://stackoverflow.com/a/31702064)
#include(ExternalProject)
#ExternalProject_Add(shadow
#  SOURCE_DIR ${CMAKE_ROOT_SOURCE_DIR}/shadow
#  BUILD_ALWAYS 1
#  CMAKE_ARGS "-DCMAKE_EXTRA_INCLUDES=$ENV{HOME}/.shadow/include" "-DCMAKE_EXTRA_LIBRARIES=$ENV{HOME}/.shadow/lib" "-DCMAKE_INSTALL_PREFIX=$ENV{HOME}/.shadow")
## set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/.shadow")
## set(CMAKE_EXTRA_INCLUDES "$ENV{HOME}/.shadow/include") # for glib-2.42
## set(CMAKE_EXTRA_LIBRARIES "$ENV{HOME}/.shadow/lib") # for glib-2.42
### TODO if we have cmake > 3.1, can we do the following?
## find_package (PkgConfig REQUIRED)
## pkg_check_modules(GLIB REQUIRED glib-2.0>=2.42)
## message(STATUS "GLIB_VERSION = ${GLIB_VERSION}")
## add_subdirectory(shadow)

find_package (PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED glib-2.0>=2.42)
message(STATUS "GLIB_VERSION = ${GLIB_VERSION}")
add_subdirectory(shadow)



## add BLEEP
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/Install")
enable_testing()


add_subdirectory(external)

add_subdirectory(BLEEPlib)
# add_subdirectory(BLEEPapp)

add_subdirectory(tests)




## add tests
# set(SHADOW_ROOT "${CMAKE_INSTALL_PREFIX}")
# enable_testing()
# add_subdirectory(test)

## add command for lauching BLEEP web interface
add_custom_target(run-web-gui
  COMMAND echo " "
  COMMAND echo "Direct launch of the server using make is not possible"
  COMMAND echo "due to the unresolved problem during relaunching process of shadow."
  COMMAND echo "Move to ${CMAKE_ROOT_SOURCE_DIR}/test/BLEEP/web-gui and type following command"
  COMMAND echo " $$ node chat-server.js"
  COMMAND echo " "
  WORKING_DIRECTORY ${CMAKE_ROOT_SOURCE_DIR}/test/BLEEP/web-gui
  )
