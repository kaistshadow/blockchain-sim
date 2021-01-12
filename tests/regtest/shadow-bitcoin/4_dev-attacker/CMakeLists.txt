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

find_package(Libev_4_33 REQUIRED) # libev4.33 is required because BLEEP uses libev4.33..  TODO: export libev4.33 when the BLEEP is exported?
include(ShadowTools) # load ShadowTools Module

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

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


################################################## for attacker-bind, sender-bind
add_custom_target(shadow-bitcoin-dev-attacker-bind-prepare ALL
        COMMAND rm -rf data/bcdnode2-bind
        COMMAND mkdir -p data/bcdnode2-bind
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

## attacker-bind
add_shadow_plugin(shadow-bitcoin-dev-attacker-bind
        attacker-bind.cpp)
target_link_libraries(shadow-bitcoin-dev-attacker-bind NEWBLEEP)
target_link_libraries(shadow-bitcoin-dev-attacker-bind BLEEP)
# copy resulting plugins into source directory
add_custom_command(TARGET shadow-bitcoin-dev-attacker-bind
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:shadow-bitcoin-dev-attacker-bind> ${CMAKE_CURRENT_SOURCE_DIR})

## sender-bind
add_shadow_plugin(shadow-bitcoin-dev-sender-bind
        sender-bind.cpp)
target_link_libraries(shadow-bitcoin-dev-sender-bind NEWBLEEP)
target_link_libraries(shadow-bitcoin-dev-sender-bind BLEEP)
# copy resulting plugins into source directory
add_custom_command(TARGET shadow-bitcoin-dev-sender-bind
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:shadow-bitcoin-dev-sender-bind> ${CMAKE_CURRENT_SOURCE_DIR})


