## required for independent build
cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)
if (NOT TARGET BLEEP) # check whether it's independent build
  list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../Install/cmake") # Path for BLEEP Module
  find_package(BLEEP REQUIRED) # load BLEEP Module
endif()
## required for independent build end

## include shadow 
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../shadow/cmake") # Path for ShadowTool Module
include(ShadowTools) # load ShadowTools Module

## enable CTEST
enable_testing()


add_shadow_plugin(NODE_BL_TEST_TX_10 node_bl_test_tx.cpp)
target_link_libraries(NODE_BL_TEST_TX_10 NEWBLEEP)
target_link_libraries(NODE_BL_TEST_TX_10 BLEEP)
  
add_test(NAME regtest-BLEEP-gossip-10node
  COMMAND python test-broadcast.py --noserver 10node-txgossip.xml
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/")
  
set_property(TEST regtest-BLEEP-gossip-10node APPEND
  PROPERTY
  ENVIRONMENT LD_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}:$ENV{LD_LIBRARY_PATH})
