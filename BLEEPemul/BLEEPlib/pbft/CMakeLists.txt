if (NOT CMAKE_INSTALL_PREFIX OR
CMAKE_INSTALL_PREFIX STREQUAL "/usr/local")
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/../../../Install")
endif()
## required for independent build
cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)

## enable CTEST
enable_testing()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../Install/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../shadow/cmake")

#add_custom_target(poslib-prepare ALL
#  COMMAND mkdir -p data/
#  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_shadow_plugin(BLEEPemul-test-pbft-node node-pbft.cpp)
target_link_libraries(BLEEPemul-test-pbft-node BLEEP)
# copy resulting plugins into source directory
add_custom_command(TARGET BLEEPemul-test-pbft-node
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BLEEPemul-test-pbft-node> ${CMAKE_CURRENT_SOURCE_DIR})
