## ensure cmake version
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

# cmake_policy(SET CMP0048 NEW)
# project(BLEEPlib VERSION 1.0 LANGUAGES CXX)

project(BLEEPlib)


## ensure out-of-source build
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "An out-of-source build is required. Please see the README.")
endif("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")


set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/.bleep")

# debug option
if(BLEEP_DEBUG STREQUAL ON)
  message(STATUS "Debug enabled.")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -std=c++11 -Wall -Werror -Wno-delete-non-virtual-dtor -Wno-deprecated-declarations -Wno-unused-local-typedefs -Wno-address") # suppress error for manually installed boost (1.58)
else(BLEEP_DEBUG STREQUAL ON)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Werror -Wno-delete-non-virtual-dtor -Wno-deprecated-declarations -Wno-unused-local-typedefs -Wno-address")
endif(BLEEP_DEBUG STREQUAL ON)

set(SRCS
  src/datamodules/Block.cpp
  src/datamodules/ShadowPipe.cpp
  src/datamodules/Pipe.cpp
  src/datamodules/Socket.cpp
  src/datamodules/Message.cpp
  src/datamanagermodules/TxPool.cpp
  src/datamanagermodules/ShadowPipeManager.cpp
  src/datamanagermodules/PipeManager.cpp
  src/datamanagermodules/SocketManager.cpp
  src/datamanagermodules/PeerManager.cpp
  src/mainmodules/MainEventManager.cpp
  src/mainmodules/BasicNetworkModule.cpp
  src/mainmodules/RandomGossipNetworkModule.cpp
  src/mainmodules/EstimateRTTModule.cpp
  src/mainmodules/TxGeneratorModule.cpp
  src/mainmodules/POWModule.cpp
  src/utility/UInt256.cpp
  src/utility/Assert.cpp
  src/utility/Random.cpp
  src/utility/ArgsManager.cpp
  src/utility/tinyxml2.cpp
  src/datamodules/Socket_v2.cpp
  src/datamanagermodules/SocketManager_v2.cpp
  src/datamodules/POWBlock.cpp
  src/crypto/SHA256.cpp
  src/utility/GlobalClock.cpp
)

include(GNUInstallDirs)

message(STATUS "CMAKE_INSTALL_LIBDIR:" ${CMAKE_INSTALL_LIBDIR})
message(STATUS "CMAKE_INSTALL_INCLUDEDIR:" ${CMAKE_INSTALL_INCLUDEDIR})


set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH}:${CMAKE_INSTALL_PREFIX}/lib:${CMAKE_CURRENT_SOURCE_DIR}/boost/lib")

add_library(BLEEP SHARED ${SRCS})
# target_include_directories(BLEEP
#   PRIVATE src
#   INTERFACE src)
target_include_directories(BLEEP
  PUBLIC 
  $<INSTALL_INTERFACE:include> 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/shadow_interface>)


# use 3rd party library (boost_1_58)
find_package(Boost 1.58.0 EXACT COMPONENTS serialization filesystem system)
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
  NAMESPACE BLEEP::
  DESTINATION cmake)
# install(FILES "src/datamanagermodules/LedgerManager.h"
#   DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/datamanagermodules)
install(DIRECTORY "src/datamanagermodules"
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  FILES_MATCHING
  PATTERN
  "*.h")
install(DIRECTORY "src/datamodules"
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  FILES_MATCHING
  PATTERN
  "*.h")
install(DIRECTORY "src/mainmodules"
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  FILES_MATCHING
  PATTERN
  "*.h")
install(DIRECTORY "src/utility"
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  FILES_MATCHING
  PATTERN
  "*.h")
install(DIRECTORY "src/crypto"
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  FILES_MATCHING
  PATTERN
  "*.h")
target_link_libraries(BLEEP -lev)
target_link_libraries(BLEEP SHADOW_INTERFACE)
target_link_libraries(BLEEP ${Boost_LIBRARIES})

include_directories(./shadow_interface)
include_directories(${Boost_INCLUDE_DIRS})

add_subdirectory(shadow_interface)

# add tests
# enable_testing()
# add_subdirectory(libtest)
