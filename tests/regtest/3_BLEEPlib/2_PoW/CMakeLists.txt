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


add_shadow_plugin(BLEEPlib-test-pow-peer test-pow-peer.cpp)
target_link_libraries(BLEEPlib-test-pow-peer
        BLEEP)
# copy resulting plugins into source directory
add_custom_command(TARGET BLEEPlib-test-pow-peer
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BLEEPlib-test-pow-peer> ${CMAKE_CURRENT_SOURCE_DIR})

# Testcase for ConnectPeer API
add_test(NAME BLEEPlib-pow-peer-1-ConnectPeer
        COMMAND $<TARGET_FILE:shadow> test-ConnectPeer.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# Testcase for Mining API
add_test(NAME BLEEPlib-pow-peer-2-MiningWithPeer
        COMMAND $<TARGET_FILE:shadow> test-MiningWithPeer.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
