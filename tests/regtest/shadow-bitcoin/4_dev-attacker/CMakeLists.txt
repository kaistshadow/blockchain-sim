## required for independent build
cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)
if (NOT TARGET BLEEP) # check whether it's independent build
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Install/cmake") # Path for BLEEP Module
    find_package(BLEEP REQUIRED) # load BLEEP Module
endif()
## enable CTEST
enable_testing()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Install/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../shadow/cmake")
include(ShadowTools) # load ShadowTools Module

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
link_directories(/home/ilios/opt/libev-4.33/lib)
include_directories(/home/ilios/opt/libev-4.33/include)

find_package(Bitcoin_0_19_1dev REQUIRED)

add_custom_target(shadow-bitcoin-dev-attacker-prepare ALL
  COMMAND rm -rf data/bcdnode2
  COMMAND mkdir -p data/bcdnode2
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(shadow-bitcoin-dev-attacker-copy-plugin ALL
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Bitcoin_0_19_1dev::Plugin> ${CMAKE_CURRENT_SOURCE_DIR})

add_shadow_plugin(shadow-bitcoin-dev-attacker
        attacker.cpp)
target_link_libraries(shadow-bitcoin-dev-attacker NEWBLEEP)
target_link_libraries(shadow-bitcoin-dev-attacker BLEEP)

# copy resulting plugins into source directory
add_custom_command(TARGET shadow-bitcoin-dev-attacker
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:shadow-bitcoin-dev-attacker> ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME shadow-bitcoin-dev-attacker
        COMMAND shadow test-attacker.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set_tests_properties(shadow-bitcoin-dev-attacker PROPERTIES TIMEOUT 60)
