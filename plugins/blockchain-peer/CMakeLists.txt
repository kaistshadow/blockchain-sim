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

set(STELLAR_SRCS
  consensus/stellarconsensus.cpp
  consensus/stellarconsensus2.cpp
  consensus/stellarquorum.cpp
  consensus/stellarconsensusvalue.cpp
  crypto/sha256.cpp
  consensus/stellarconsensusdriver.cpp
)

set(POW_SRCS
  blockchain/powledgermanager.cpp
  blockchain/powblock.cpp
  consensus/powconsensus.cpp
)

set(SRCS
  p2p/simplepeerlist.cpp
  p2p/gossipprotocol.cpp
  p2p/nodeinfos.cpp
  p2p/socket.cpp
  p2p/socketmessage.cpp
  p2p/plumtree.cpp
  blockchain/block.cpp
  blockchain/transaction.cpp
  blockchain/txpool.cpp
  blockchain/ledgermanager.cpp
  consensus/simpleconsensus.cpp
  util/eventqueue.cpp
  util/globalclock.cpp
  util/types.cpp
  ${STELLAR_SRCS}
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



## create and install a shared library that can plug into shadow
add_shadow_plugin(BLOCKCHAIN_PEER_SIMPLE_CONSENSUS node_simpleconsensus.cpp ${SRCS})
install(TARGETS BLOCKCHAIN_PEER_SIMPLE_CONSENSUS DESTINATION plugins)

##
# add_executable(PEER_NONBLOCK_IO_LOCAL node_nonblock_io.cpp ${SRCS})
add_shadow_plugin(PEER_NONBLOCK_IO node_nonblock_io.cpp ${SRCS})
install(TARGETS PEER_NONBLOCK_IO DESTINATION plugins)
# target_link_libraries(PEER_NONBLOCK_IO_LOCAL ${Boost_LIBRARIES})
target_link_libraries(PEER_NONBLOCK_IO ${Boost_LIBRARIES})


##
# add_executable(INJECTOR_SIMPLE_LOCAL node_injector_simple.cpp ${SRCS})
add_shadow_plugin(INJECTOR_SIMPLE node_injector_simple.cpp ${SRCS})
install(TARGETS INJECTOR_SIMPLE DESTINATION plugins)
# target_link_libraries(INJECTOR_SIMPLE_LOCAL ${Boost_LIBRARIES})
target_link_libraries(INJECTOR_SIMPLE ${Boost_LIBRARIES})


##
# add_executable(PEER_SIMPLECONSENSUS_LOCAL node_simpleconsensus.cpp ${SRCS})
add_shadow_plugin(PEER_SIMPLECONSENSUS node_simpleconsensus.cpp ${SRCS})
install(TARGETS PEER_SIMPLECONSENSUS DESTINATION plugins)
# target_link_libraries(PEER_SIMPLECONSENSUS_LOCAL ${Boost_LIBRARIES})
target_link_libraries(PEER_SIMPLECONSENSUS ${Boost_LIBRARIES})

# ##
add_shadow_plugin(INJECTOR_SIMPLECONSENSUS node_injector_simpleconsensus.cpp ${SRCS})
install(TARGETS INJECTOR_SIMPLECONSENSUS DESTINATION plugins)
target_link_libraries(INJECTOR_SIMPLECONSENSUS ${Boost_LIBRARIES})


## stellar consensus
# add_executable(PEER_STELLARCONSENSUS_LOCAL node_simpleconsensus.cpp ${SRCS})
# add_shadow_plugin(PEER_STELLARCONSENSUS node_stellarconsensus.cpp ${SRCS})
# install(TARGETS PEER_STELLARCONSENSUS DESTINATION plugins)
# target_link_libraries(PEER_STELLARCONSENSUS_LOCAL ${Boost_LIBRARIES})
# target_link_libraries(PEER_STELLARCONSENSUS ${Boost_LIBRARIES})

# add_executable(INJECTOR_STELLARCONSENSUS_LOCAL node_injector_stellarconsensus.cpp ${SRCS})
# add_shadow_plugin(INJECTOR_STELLARCONSENSUS node_injector_stellarconsensus.cpp ${SRCS})
# install(TARGETS INJECTOR_STELLARCONSENSUS DESTINATION plugins)
# target_link_libraries(INJECTOR_STELLARCONSENSUS ${Boost_LIBRARIES})
# target_link_libraries(INJECTOR_STELLARCONSENSUS_LOCAL ${Boost_LIBRARIES})


## dump blockchain 
# add_executable(DUMP_BLOCKCHAIN_LOCAL dump_blockchain.cpp ${SRCS})
add_shadow_plugin(DUMP_BLOCKCHAIN dump_blockchain.cpp ${SRCS})
install(TARGETS DUMP_BLOCKCHAIN DESTINATION plugins)
target_link_libraries(DUMP_BLOCKCHAIN ${Boost_LIBRARIES})
# target_link_libraries(DUMP_BLOCKCHAIN_LOCAL ${Boost_LIBRARIES})

## POW blockchain 
add_executable(PEER_POWCONSENSUS_LOCAL node_powconsensus.cpp ${SRCS} ${POW_SRCS})
add_shadow_plugin(PEER_POWCONSENSUS node_powconsensus.cpp ${SRCS} ${POW_SRCS})
install(TARGETS PEER_POWCONSENSUS DESTINATION plugins)
target_link_libraries(PEER_POWCONSENSUS_LOCAL ${Boost_LIBRARIES})
target_link_libraries(PEER_POWCONSENSUS ${Boost_LIBRARIES})

## POW blockchain
add_shadow_plugin(INJECTOR_POWCONSENSUS node_injector_powconsensus.cpp ${SRCS})
install(TARGETS INJECTOR_POWCONSENSUS DESTINATION plugins)
target_link_libraries(INJECTOR_POWCONSENSUS ${Boost_LIBRARIES})

## dump POW blockchain 
add_shadow_plugin(DUMP_POWBLOCKCHAIN dump_powblockchain.cpp ${SRCS} ${POW_SRCS})
install(TARGETS DUMP_POWBLOCKCHAIN DESTINATION plugins)
target_link_libraries(DUMP_POWBLOCKCHAIN ${Boost_LIBRARIES})

##
add_shadow_plugin(PEER_PLUMTREE node_membershiptest.cpp ${SRCS})
install(TARGETS PEER_PLUMTREE DESTINATION plugins)
target_link_libraries(PEER_PLUMTREE ${Boost_LIBRARIES})

