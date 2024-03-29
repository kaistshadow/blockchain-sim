## required for independent build
cmake_minimum_required(VERSION 2.8...3.13)
if (NOT TARGET BLEEP) # check whether it's independent build
  list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../Install/cmake") # Path for BLEEP Module
  find_package(BLEEP REQUIRED) # load BLEEP Module
endif()
## required for independent build end

## include shadow 
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../shadow/cmake") # Path for ShadowTool Module
include(ShadowTools) # load ShadowTools Module

## enable CTEST
enable_testing()


add_shadow_plugin(NODE_BL_TEST_TX_10 node_bl_test_tx.cpp)
find_package(Libev_4_33 REQUIRED)
target_link_libraries(NODE_BL_TEST_TX_10 BLEEP)

# copy resulting plugins into source directory
add_custom_command(TARGET NODE_BL_TEST_TX_10
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:NODE_BL_TEST_TX_10> ${CMAKE_CURRENT_SOURCE_DIR})


add_test(NAME regtest-BLEEP-gossip-10node
        COMMAND python test-broadcast.py --noserver 10node-txgossip.xml
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/")

set_property(TEST regtest-BLEEP-gossip-10node APPEND
        PROPERTY
        ENVIRONMENT LD_LIBRARY_PATH=${CMAKE_CURRENT_SOURCE_DIR}:$ENV{LD_LIBRARY_PATH}
        PATH=$ENV{PATH}:${CMAKE_CURRENT_SOURCE_DIR}/../../../Install/bin) # add path environment in order to use CLion


## test for peer connection (Node Discovery Protocol)
add_shadow_plugin(NODE_BL_TEST_PEER node_bl_test_peerconnection.cpp)
find_package(Libev_4_33 REQUIRED)
target_link_libraries(NODE_BL_TEST_PEER BLEEP)

add_custom_command(TARGET NODE_BL_TEST_PEER
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:NODE_BL_TEST_PEER> ${CMAKE_CURRENT_SOURCE_DIR})
# run following command for visualization
# python test-broadcast.py --nocheck 10node-peerconnection-ring.xml
