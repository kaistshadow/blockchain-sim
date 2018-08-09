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

set(SRCS
  p2p/simplepeerlist.cpp
  p2p/gossipprotocol.cpp
  p2p/nodeinfos.cpp
  consensus/simpleconsensus.cpp
)
  

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
set(CMAKE_MODULE_PATH "${SHADOW_ROOT}/share/cmake/Modules")
if(EXISTS "${CMAKE_MODULE_PATH}")
  message(STATUS "CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")
else()
  message(FATAL_ERROR "CMAKE_MODULE_PATH path does not exist: '${CMAKE_MODULE_PATH}'")
endif()
include(ShadowTools)

## create and install a shared library that can plug into shadow
add_shadow_plugin(BLOCKCHAIN_PEER_EXAMPLE node_example.cpp ${SRCS})
# target_link_libraries(shadow-plugin-hello)
install(TARGETS BLOCKCHAIN_PEER_EXAMPLE DESTINATION plugins)

## create local executable
add_executable(BLOCKCHAIN_PEER_EXAMPLE_LOCAL node_example.cpp ${SRCS})


## 
add_shadow_plugin(BLOCKCHAIN_PEER_SIMPLE_CONSENSUS node_simpleconsensus.cpp ${SRCS})
install(TARGETS BLOCKCHAIN_PEER_SIMPLE_CONSENSUS DESTINATION plugins)

##
add_executable(PEER_NONBLOCK_IO_LOCAL node_nonblock_io.cpp ${SRCS})
add_shadow_plugin(PEER_NONBLOCK_IO node_nonblock_io.cpp ${SRCS})
install(TARGETS PEER_NONBLOCK_IO DESTINATION plugins)

##
add_executable(INJECTOR_SIMPLE_LOCAL node_injector_simple.cpp ${SRCS})
add_shadow_plugin(INJECTOR_SIMPLE node_injector_simple.cpp ${SRCS})
install(TARGETS INJECTOR_SIMPLE DESTINATION plugins)


