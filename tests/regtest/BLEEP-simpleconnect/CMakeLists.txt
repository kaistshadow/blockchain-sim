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


add_shadow_plugin(NODE_BL_TEST node_bl_test.cpp)
target_link_libraries(NODE_BL_TEST BLEEP)

# copy resulting plugins into source directory
add_custom_command(TARGET NODE_BL_TEST
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:NODE_BL_TEST> ${CMAKE_CURRENT_SOURCE_DIR})


add_test(NAME regtest-BLEEP-simpleconnect
        COMMAND shadow simple-connect.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
