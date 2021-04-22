## required for independent build
cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)


## enable CTEST
enable_testing()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Install/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../shadow/cmake")

#include(ShadowTools) # load ShadowTools Module
#find_package(zcash_dev_ REQUIRED)
message("-------------------------------------")
message( ${CMAKE_HOME_DIRECTORY})


add_custom_target(copy-litecoin-plugin ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:litecoin_dev_::Plugin> ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME shadow-litecoin
        COMMAND shadow example.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})


