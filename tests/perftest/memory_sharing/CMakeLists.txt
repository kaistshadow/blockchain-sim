## required for independent build
cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../Install/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../shadow/cmake")

find_package(shadow REQUIRED)
find_package(Libev_4_33 REQUIRED)
include(ShadowTools) # load ShadowTools Module
add_shadow_plugin(BLEEPlib-test-pow-node node-pow.cpp)
target_link_libraries(BLEEPlib-test-pow-node ${CMAKE_CURRENT_SOURCE_DIR}/../../../Install/lib/libBLEEP.so)
include_directories(BLEEPlib-test-pow-node ${CMAKE_CURRENT_SOURCE_DIR}/../../../BLEEPlib/src)
include_directories(BLEEPlib-test-pow-node ${CMAKE_CURRENT_SOURCE_DIR}/../../../Install/include)
# copy resulting plugins into source directory
add_custom_command(TARGET BLEEPlib-test-pow-node
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BLEEPlib-test-pow-node> ${CMAKE_CURRENT_SOURCE_DIR})