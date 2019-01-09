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

set(SRCS
  p2pnetwork/HandleNetwork.cpp
  p2pnetwork/HandleNetwork_ProxyModel.cpp
  datamodules/Peer.cpp
  datamodules/Block.cpp
  datamodules/POWBlock.cpp
  datamanagermodules/POWLedgerManager.cpp
  datamanagermodules/TxPool.cpp
  event/GlobalEvent.cpp
  event/EventCoordinator.cpp
  event/Publisher.cpp
  utility/GlobalClock.cpp
  utility/NodeInfo.cpp
  utility/ArgsManager.cpp
  utility/UInt256.cpp
  crypto/SHA256.cpp
  transaction/HandleTransaction.cpp
  transaction/HandleTransaction_BasicModel.cpp
  consensus/HandleConsensus.cpp
  consensus/HandleConsensus_POW.cpp
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
if(EXISTS "${CMAKE_MODULE_PATH}")
  message(STATUS "CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")
else()
  set(CMAKE_MODULE_PATH "${SHADOW_ROOT}/share/cmake/Modules")
  message(STATUS "CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")
  # message(FATAL_ERROR "CMAKE_MODULE_PATH path does not exist: '${CMAKE_MODULE_PATH}'")
endif()
include(ShadowTools)


# add_executable(HELLO hello.cpp ${SRCS})

add_shadow_plugin(NODE node.cpp ${SRCS})
install(TARGETS NODE DESTINATION plugins)
target_link_libraries(NODE -lev)
target_link_libraries(NODE ${Boost_LIBRARIES})

add_executable(LOCAL_NODE node.cpp ${SRCS})
target_link_libraries(LOCAL_NODE -lev)
target_link_libraries(LOCAL_NODE ${Boost_LIBRARIES})
