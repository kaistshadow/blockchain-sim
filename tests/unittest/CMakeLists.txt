cmake_minimum_required(VERSION 3.5)

set(This GTest-ExampleTests)

set(SRCS
        GTest-ExampleTests.cpp)

add_executable(${This} ${SRCS})
target_link_libraries(${This} PUBLIC
        gtest_main
        BLEEP)

install(TARGETS ${This} DESTINATION unittest)

add_test(
        NAME ${This}
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${This}
#        COMMAND ${CMAKE_INSTALL_PREFIX}/unittest/${This}
)


# Example for catch2 unit test
add_library(Catch INTERFACE)
target_include_directories(Catch
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(catch-example catch-example.cpp)
target_link_libraries(catch-example
        Catch
        BLEEP)


## include shadow
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../shadow/cmake") # Path for ShadowTool Module
include(ShadowTools) # load ShadowTools Module

## enable CTEST
enable_testing()


add_executable(NODE_BL_TEST2 node_bl_test.cpp)
target_link_libraries(NODE_BL_TEST2 BLEEP)

# copy resulting plugins into source directory
add_custom_command(TARGET NODE_BL_TEST2
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:NODE_BL_TEST2> ${CMAKE_CURRENT_SOURCE_DIR})

