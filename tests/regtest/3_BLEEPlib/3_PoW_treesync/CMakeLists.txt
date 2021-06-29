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

add_custom_target(BLEEPlib-treesynctest-pow-node ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BLEEPlib-test-pow-node> ${CMAKE_CURRENT_SOURCE_DIR})
# copy resulting plugins into source directory
add_custom_command(TARGET BLEEPlib-test-pow-node
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BLEEPlib-test-pow-node> ${CMAKE_CURRENT_SOURCE_DIR}
        #COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BLEEPlib-test-pow-node> ${CMAKE_CURRENT_SOURCE_DIR}/../../perftest/memory_sharing
        )

#add_test(NAME BLEEPlib-pow-treesync-1
#        COMMAND python test-treesync.py --noserver treesynctest1-twonode.xml
#        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/")

#set_property(TEST BLEEPlib-pow-treesync-1 APPEND
#        PROPERTY
#        ENVIRONMENT PATH=$ENV{PATH}:${CMAKE_CURRENT_SOURCE_DIR}/../../../../Install/bin) # add path environment in order to use CLion


#add_test(NAME BLEEPlib-pow-treesync-2
#        COMMAND python test-treesync.py --noserver treesynctest2-tennode.xml
#        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/")

#set_property(TEST BLEEPlib-pow-treesync-2 APPEND
#        PROPERTY
#        ENVIRONMENT PATH=$ENV{PATH}:${CMAKE_CURRENT_SOURCE_DIR}/../../../../Install/bin) # add path environment in order to use CLion

#
## Testcase for ConnectPeer API
#add_test(NAME BLEEPlib-pow-peer-1-ConnectPeer
#        COMMAND $<TARGET_FILE:shadow> test-ConnectPeer.xml
#        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
#
## Testcase for Mining API
#add_test(NAME BLEEPlib-pow-peer-2-BlockInvPropagate
#        COMMAND $<TARGET_FILE:shadow> test-BlockInvPropagate.xml
#        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
