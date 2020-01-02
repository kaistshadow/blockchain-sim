project(BLEEP)

## ensure cmake version
cmake_minimum_required(VERSION 2.8)

## ensure out-of-source build
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "An out-of-source build is required. Please see the README.")
endif("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")

# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# should we use our own math functions?
option (SHADOW
    "Compile as a shadow plugin" OFF) 

message("SHADOW:" ${SHADOW})

find_package(Boost REQUIRED serialization filesystem system)

message(STATUS "Boost_LIBRARIES:" ${Boost_LIBRARIES})

include_directories(${Boost_INCLUDE_DIRS})
include_directories(./shadow_interface)


## we need to know where shadow is installed
if("${SHADOW_ROOT}" STREQUAL "")
  set(SHADOW_ROOT "$ENV{HOME}/.shadow")
endif("${SHADOW_ROOT}" STREQUAL "")
if(EXISTS "${SHADOW_ROOT}")
  message(STATUS "SHADOW_ROOT = ${SHADOW_ROOT}")
else()
  message(FATAL_ERROR "SHADOW_ROOT path does not exist: '${SHADOW_ROOT}'")
endif()

set(CMAKE_INSTALL_PREFIX "${SHADOW_ROOT}")

if(EXISTS "${CMAKE_INSTALL_PREFIX}")
  message(STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")
else()
  message(FATAL_ERROR "CMAKE_INSTALL_PREFIX path does not exist: '${CMAKE_INSTALL_PREFIX}'")
endif()

## we use cmake files from shadow
if(EXISTS "${CMAKE_MODULE_PATH}")
  message(STATUS "CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")
else()
  set(CMAKE_MODULE_PATH "${SHADOW_ROOT}/share/cmake/Modules")
  message(STATUS "CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")
  # message(FATAL_ERROR "CMAKE_MODULE_PATH path does not exist: '${CMAKE_MODULE_PATH}'")
endif()
include(ShadowTools)


## set BLEEP source root directory
if(EXISTS "${BLEEP_SRC_ROOT}")
  message(STATUS "BLEEP_SRC_ROOT = ${BLEEP_SRC_ROOT}")
else()
  set(BLEEP_SRC_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
  message(STATUS "BLEEP_SRC_ROOT = ${BLEEP_SRC_ROOT}")
  # message(FATAL_ERROR "CMAKE_MODULE_PATH path does not exist: '${CMAKE_MODULE_PATH}'")
endif()


if(BLEEP_DEBUG STREQUAL ON)
  message(STATUS "Debug enabled.")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -std=c++11 -Wall -Werror")
else(BLEEP_DEBUG STREQUAL ON)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Werror")
endif(BLEEP_DEBUG STREQUAL ON)


## include libBLEEP
set(LIBBLEEP_INCLUDE_DIR "${BLEEP_SRC_ROOT}/libBLEEP-1.0")
message(STATUS "LIBBLEEP_INCLUDE_DIR = ${LIBBLEEP_INCLUDE_DIR}")
include_directories(${LIBBLEEP_INCLUDE_DIR})

## compile for shadow example
add_shadow_plugin(NODE_bleep node.cpp)
install(TARGETS NODE_bleep DESTINATION plugins)
target_link_libraries(NODE_bleep BLEEP-1.0)

add_shadow_plugin(NODE_client client.cpp)
install(TARGETS NODE_client DESTINATION plugins)
target_link_libraries(NODE_client BLEEP-1.0)

## compile for local executable
add_executable(LOCAL_NODE_bleep node.cpp)
target_link_libraries(LOCAL_NODE_bleep BLEEP-1.0)

add_executable(LOCAL_NODE_client client.cpp)
target_link_libraries(LOCAL_NODE_client BLEEP-1.0)


add_subdirectory(shadow_interface)

add_subdirectory(libBLEEP-1.0)

add_subdirectory(regtest-libBLEEP)

add_subdirectory(test_nodes)