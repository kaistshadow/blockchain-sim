## required for independent build
cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)

## enable CTEST
enable_testing()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Install/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../shadow/cmake")
include(ShadowTools) # load ShadowTools Module


find_package(CURL_7_70_0 REQUIRED)

# message("CURL_7_70_0_INCLUDE_DIR=${CURL_7_70_0_INCLUDE_DIR}")
# message("CURL_7_70_0_LIBRARIES=${CURL_7_70_0_LIBRARIES}")
# message("CURL_7_70_0_FOUND=${CURL_7_70_0_FOUND}")

add_shadow_plugin(simplecurl simplecurl.c)
target_link_libraries(simplecurl CURL_7_70_0::CURL)

# copy resulting plugins into source directory
add_custom_command(TARGET simplecurl
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:simplecurl> ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME shadow-custom-libcurl
        COMMAND shadow simplecurl.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
