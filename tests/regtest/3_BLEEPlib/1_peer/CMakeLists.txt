## required for independent build
cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)
if (NOT TARGET BLEEP) # check whether it's independent build
    project(localtest)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Install/cmake") # Path for BLEEP Module
    find_package(BLEEP REQUIRED) # load BLEEP Module
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../cmake")
    find_package(shadow REQUIRED)
    find_package(Libev_4_33 REQUIRED)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../shadow/cmake")
    include(ShadowTools) # load ShadowTools Module
endif()
## enable CTEST
enable_testing()


add_shadow_plugin(BLEEPlib-test-peer test-peer.cpp)
target_link_libraries(BLEEPlib-test-peer
        BLEEP)
# copy resulting plugins into source directory
add_custom_command(TARGET BLEEPlib-test-peer
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BLEEPlib-test-peer> ${CMAKE_CURRENT_SOURCE_DIR})

# Testcase for ConnectPeer API
add_test(NAME BLEEPlib-peer-1-ConnectPeer
        COMMAND $<TARGET_FILE:shadow> test-ConnectPeer.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# Testcase for ConnectPeer API when there's no target node (no internet node)
add_test(NAME BLEEPlib-peer-2-ConnectPeerToVoid
        COMMAND $<TARGET_FILE:shadow> test-ConnectPeerToVoid1.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# Testcase for ConnectPeer API when there's no target node (internet node exists but immediatly halts)
add_test(NAME BLEEPlib-peer-3-ConnectPeerToVoid
        COMMAND $<TARGET_FILE:shadow> test-ConnectPeerToVoid2.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# Testcase for DisconnectPeer API. Call DisconnectPeer API immediately after the call of ConnectPeer API
add_test(NAME BLEEPlib-peer-4-DisconnectPeer
        COMMAND $<TARGET_FILE:shadow> test-DisconnectPeer.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# Testcase for peer terminated by exit.
# Test failed possibly due to the unhandled socket while exit
#add_test(NAME BLEEPlib-peer-5-PeerWithExit
#        COMMAND $<TARGET_FILE:shadow> test-PeerWithExit.xml
#        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# Testcase for peer terminated with explicit close
add_test(NAME BLEEPlib-peer-6-PeerWithCloseExit
        COMMAND $<TARGET_FILE:shadow> test-PeerWithCloseExit.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# Testcase for peer terminated with explicit stop(shadow's configuration)
add_test(NAME BLEEPlib-peer-7-PeerWithStop
        COMMAND $<TARGET_FILE:shadow> test-PeerWithStop.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# Testcase for peer terminated with explicit stop(shadow's configuration) for two process
# Test failed due to the listening port conflict
#add_test(NAME BLEEPlib-peer-8-PeerWithStopTwoProc
#        COMMAND $<TARGET_FILE:shadow> test-PeerWithStop-TwoProc.xml
#        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
