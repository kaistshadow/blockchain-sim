## ensure cmake version
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

cmake_policy(SET CMP0048 NEW)
project(BLEEPlib VERSION 1.0 LANGUAGES CXX)

project(BLEEPlib)

add_compile_options(
        $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<VERSION_GREATER_EQUAL:$<CXX_COMPILER_VERSION>,9.3.0>>:-std=c++2a>
        $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<VERSION_LESS:$<CXX_COMPILER_VERSION>,9.3.0>>:-std=c++17>)

## ensure out-of-source build
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "An out-of-source build is required. Please see the README.")
endif("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")


set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/../Install")

# debug option
if(BLEEP_DEBUG STREQUAL ON)
  message(STATUS "Debug enabled.")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -Wall -Werror -Wno-delete-non-virtual-dtor -Wno-parentheses -Wno-deprecated-declarations -Wno-unused-local-typedefs -Wno-address -Wno-switch") # suppress error for manually installed boost (1.58)
else(BLEEP_DEBUG STREQUAL ON)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wno-delete-non-virtual-dtor -Wno-parentheses -Wno-deprecated-declarations -Wno-unused-local-typedefs -Wno-address -Wno-switch")
endif(BLEEP_DEBUG STREQUAL ON)

include(GNUInstallDirs)

message(STATUS "CMAKE_INSTALL_LIBDIR:" ${CMAKE_INSTALL_LIBDIR})
message(STATUS "CMAKE_INSTALL_INCLUDEDIR:" ${CMAKE_INSTALL_INCLUDEDIR})


set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH}:${CMAKE_INSTALL_PREFIX}/lib:${CMAKE_CURRENT_SOURCE_DIR}/boost/lib")

add_library(BLEEP SHARED)

target_sources(BLEEP
        PRIVATE
        src/BL_MainEventManager.cpp
        src/BL_MainEventManager.h
        src/BL1_socket/Socket.cpp
        src/BL1_socket/SocketLayer_API.cpp
        src/BL1_socket/SocketManager.cpp
        src/BL1_socket/RecvBufferManager.cpp
        src/BL1_socket/SocketLayer_Bitcoin.cpp
        src/BL2_peer_connectivity/PeerConnectivityLayer_API.cpp
        src/BL2_peer_connectivity/PeerManager.cpp
        src/BL2_peer_connectivity/AddrManager.cpp
        src/BL2_peer_connectivity/AdvertisementManager.cpp
        src/BL3_protocol/ProtocolLayer_API.cpp
        src/BL3_protocol/ProtocolLayer_API.h
        src/BL3_protocol/ProtocolLayerEx1.cpp
        src/BL3_protocol/ProtocolLayerEx1.h
        src/BL3_protocol/ProtocolLayerPoW.cpp
        src/BL3_protocol/ProtocolLayerPoW.h
        src/BL3_protocol/POWProtocolParameter.h
        src/BL3_protocol/Block.h
        src/BL3_protocol/BlockTree.h
        src/BL3_protocol/BlockTree.cpp
        src/BL3_protocol/POWBlock.h
        src/BL3_protocol/POWBlock.cpp
        src/BL3_protocol/POWMiner.cpp
        src/BL3_protocol/POWBlockGossipProtocolMsg.h
        src/BL3_protocol/TxGossipProtocol.cpp
        src/BL3_protocol/TxGossipProtocol.h
        src/BL3_protocol/TxGossipProtocolMsg.h
        src/BL3_protocol/TxPool.cpp
        src/BL3_protocol/PBFTBlock.cpp
        src/BL3_protocol/PBFTSigning.cpp
        src/BL3_protocol/ProtocolLayerPBFT.cpp
        src/utility/UInt256.cpp
        src/utility/Assert.cpp
        src/utility/Random.cpp
        src/utility/ArgsManager.cpp
        src/utility/tinyxml2.cpp
        src/utility/Logger.cpp
        src/crypto/SHA256.cpp
        src/utility/GlobalClock.cpp)

target_include_directories(BLEEP
  PUBLIC 
  $<INSTALL_INTERFACE:include> 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)


# use 3rd party library (boost_1_58)
find_package(Boost 1.58.0 COMPONENTS serialization filesystem system)
if(NOT Boost_FOUND)
  include(ExternalProject)
  ExternalProject_Add(Boost_EXTERNAL
    PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/boost
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/boost/src
    STAMP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/boost/stamp
    CONFIGURE_COMMAND ""
    INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/boost
    URL http://sourceforge.net/projects/boost/files/boost/1.58.0/boost_1_58_0.tar.gz
    BUILD_COMMAND ./bootstrap.sh --prefix=<INSTALL_DIR> --with-libraries=system,filesystem,serialization
    BUILD_IN_SOURCE TRUE
    INSTALL_COMMAND <SOURCE_DIR>/b2 -d0 install)
  add_dependencies(BLEEP Boost_EXTERNAL)
  add_dependencies(NEWBLEEP Boost_EXTERNAL)
  set(BOOST_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/boost")
  set(Boost_INCLUDE_DIRS "${BOOST_ROOT}/include")
  set(BOOST_LIBRARY_DIRS "${BOOST_ROOT}/lib")
  # find_package(Boost REQUIRED serialization filesystem system)
  set(Boost_LIBRARIES "${BOOST_LIBRARY_DIRS}/libboost_system.so;${BOOST_LIBRARY_DIRS}/libboost_filesystem.so;${BOOST_LIBRARY_DIRS}/libboost_serialization.so")
endif()


set_target_properties(BLEEP PROPERTIES 
  SOVERSION 1)
install(TARGETS BLEEP
  EXPORT FindBLEEP
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(EXPORT FindBLEEP 
  FILE FindBLEEP.cmake
  DESTINATION cmake)

install(DIRECTORY "src/"
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  FILES_MATCHING
  PATTERN
  "*.h")


find_package(Libev_4_33 REQUIRED)
target_link_libraries(BLEEP Libev_4_33::Libev)
#target_link_libraries(BLEEP -lev)
target_link_libraries(BLEEP SHADOW_INTERFACE)
target_link_libraries(BLEEP SHADOW_MEMSHARE_INTERFACE)
target_link_libraries(BLEEP ${Boost_LIBRARIES})


include_directories(${CMAKE_ROOT_SOURCE_DIR}/interfaces/shadow_interface)
include_directories(${CMAKE_ROOT_SOURCE_DIR}/interfaces/shadow_memshare_interface)
include_directories(${Boost_INCLUDE_DIRS})

