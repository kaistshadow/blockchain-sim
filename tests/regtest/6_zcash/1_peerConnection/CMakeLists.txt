## required for independent build
cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)


## enable CTEST
enable_testing()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Install/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../shadow/cmake")

add_custom_target(shadow-zcash-prepare ALL
        COMMAND mkdir -p data/node1
        COMMAND mkdir -p data/node2
        COMMAND cd $ENV{HOME} && mkdir -p .zcash
        COMMAND cp -r zcash1.conf $ENV{HOME}/.zcash
        COMMAND cp -r zcash2.conf $ENV{HOME}/.zcash
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(copy_zcash ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:_ZCASH_DEV_::Plugin> ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(copy_zcash _ZCASH_DEV_::Plugin)

add_test(NAME zcash_peer_connection_test
        COMMAND shadow example.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
