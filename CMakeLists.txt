project(BLEEP)

## ensure cmake version
cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)

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

## add shadow project
set(CMAKE_SOURCE_DIR "${CMAKE_SOURCE_DIR}/shadow")
set(CMAKE_BINARY_DIR "${CMAKE_BINARY_DIR}/shadow")
set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/.shadow")
set(CMAKE_EXTRA_INCLUDES "$ENV{HOME}/.shadow/include") # for glib-2.42
set(CMAKE_EXTRA_LIBRARIES "$ENV{HOME}/.shadow/lib") # for glib-2.42
## TODO if we have cmake > 3.1, can we do the following?
# find_package (PkgConfig REQUIRED)
# pkg_check_modules(GLIB REQUIRED glib-2.0>=2.42)
# message(STATUS "GLIB_VERSION = ${GLIB_VERSION}")
add_subdirectory(shadow)

## add shadow-plugin-bitcoin
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
set(CMAKE_SOURCE_DIR "${CMAKE_SOURCE_DIR}/../plugins/shadow-plugin-bitcoin")
set(CMAKE_BINARY_DIR "${CMAKE_BINARY_DIR}/../")
set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/.shadow")

add_subdirectory(plugins/shadow-plugin-bitcoin)

