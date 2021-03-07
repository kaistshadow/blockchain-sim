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
