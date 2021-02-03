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

add_executable(catch-BLEEPlib catch-BLEEPlib.cpp)
target_link_libraries(catch-BLEEPlib
        Catch
        BLEEP)


add_shadow_plugin(shadow-plugin-test-peer test-peer.cpp)
target_link_libraries(shadow-plugin-test-peer
        BLEEP)
# copy resulting plugins into source directory
add_custom_command(TARGET shadow-plugin-test-peer
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:shadow-plugin-test-peer> ${CMAKE_CURRENT_SOURCE_DIR})

