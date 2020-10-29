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

add_shadow_plugin(NODE_POW_RANDOM_TREE node_pow_random_tree.cpp)
target_link_libraries(NODE_POW_RANDOM_TREE BLEEP)

add_shadow_plugin(NODE_client client.cpp)
target_link_libraries(NODE_client BLEEP)

# copy resulting plugins into source directory
add_custom_command(TARGET NODE_POW_RANDOM_TREE
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:NODE_POW_RANDOM_TREE> ${CMAKE_CURRENT_SOURCE_DIR})
add_custom_command(TARGET NODE_client
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:NODE_client> ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME regtest-BLEEP-POW-consensus
  COMMAND shadow example.xml
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
