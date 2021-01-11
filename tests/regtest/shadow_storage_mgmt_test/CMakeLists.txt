## required for independent build
cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)

add_shadow_plugin(SHADOW_STORAGE_MGMT_TEST main.cpp)
add_dependencies(SHADOW_STORAGE_MGMT_TEST SHADOW_STORAGE_MGMT)
target_link_libraries(SHADOW_STORAGE_MGMT_TEST SHADOW_STORAGE_MGMT)

target_include_directories(SHADOW_STORAGE_MGMT_TEST
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

add_custom_command(TARGET SHADOW_STORAGE_MGMT_TEST
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:SHADOW_STORAGE_MGMT_TEST> ${CMAKE_CURRENT_SOURCE_DIR})