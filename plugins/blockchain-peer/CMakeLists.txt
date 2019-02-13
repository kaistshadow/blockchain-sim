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


##
# add_executable(PEER_NONBLOCK_IO_LOCAL node_nonblock_io.cpp ${SRCS})
add_shadow_plugin(PEER_NONBLOCK_IO node_nonblock_io.cpp)
install(TARGETS PEER_NONBLOCK_IO DESTINATION plugins)
# target_link_libraries(PEER_NONBLOCK_IO_LOCAL ${Boost_LIBRARIES})
target_link_libraries(PEER_NONBLOCK_IO ${Boost_LIBRARIES})


## Asio test
add_shadow_plugin(PEER_ASIO node_asio.cpp)
install(TARGETS PEER_ASIO DESTINATION plugins)
target_link_libraries(PEER_ASIO ${Boost_LIBRARIES})

## libev test
add_shadow_plugin(PEER_LIBEV_SERV node_libev_server.cpp )
install(TARGETS PEER_LIBEV_SERV DESTINATION plugins)
target_link_libraries(PEER_LIBEV_SERV -lev)

add_shadow_plugin(PEER_LIBEV_CLIENT node_libev_client.cpp )
install(TARGETS PEER_LIBEV_CLIENT DESTINATION plugins)
target_link_libraries(PEER_LIBEV_CLIENT -lev)


add_subdirectory(rc-1.0)