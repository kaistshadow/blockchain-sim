## required for independent build
cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)


## enable CTEST
enable_testing()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../Install/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../shadow/cmake")

#include(ShadowTools) # load ShadowTools Module
find_package(EOS_2_0_7 REQUIRED)

#add_custom_target(shadow-eos-prepare ALL
#        COMMAND mkdir -p data/node1
#        COMMAND mkdir -p data/node2
#        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(copy-eos-plugin ALL
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../../../Install/plugins/libNODEOS_2.0.7.so
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:EOS_2_0_7::Plugin> ${CMAKE_CURRENT_SOURCE_DIR})
#add_dependencies(copy-eos-plugin libnodeos)

add_custom_target(copy-eos-bin ALL
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../../../external/eosio/eos/build/bin/nodeos
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:EOS_2_0_7_EXECUTABLE::Bin> ${CMAKE_CURRENT_SOURCE_DIR})
#add_dependencies(copy-eos-bin nodeos)

add_test(NAME shadow-eos
        COMMAND shadow example.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})


