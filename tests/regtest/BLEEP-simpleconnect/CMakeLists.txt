## required for independent build
cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../Install/share/cmake/Modules") # Path for ShadowTool Module
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../Install/cmake") # Path for BLEEP Module
include(ShadowTools) # load ShadowTools Module
find_package(BLEEP REQUIRED) # load BLEEP Module

enable_testing()
## required for independent build


add_shadow_plugin(NODE_BL_TEST node_bl_test.cpp)
target_link_libraries(NODE_BL_TEST BLEEP::NEWBLEEP)
target_link_libraries(NODE_BL_TEST BLEEP::BLEEP)


add_test(NAME BLEEP-regtest-simpleconnect
  COMMAND shadow ${CMAKE_CURRENT_SOURCE_DIR}/simple-connect.xml
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
  # WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/..")
