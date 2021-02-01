cmake_minimum_required(VERSION 3.5)

add_executable(test_ test.cpp)

add_custom_target(test_copy ALL
    DEPENDS test_
    COMMAND ${CMAKE_COMMAND} -E copy test_ ${CMAKE_CURRENT_SOURCE_DIR}
)