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

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

find_package(Bitcoin_0_19_1dev REQUIRED)
find_package(CURL_7_70_0 REQUIRED)
find_package(Libev_4_33 REQUIRED) # libev4.33 is required because BLEEP uses libev4.33..  TODO: export libev4.33 when the BLEEP is exported?

################################################## for bitcoin experiment
add_custom_target(shadow-ISP-bitcoin-prepare ALL
        COMMAND rm -rf data/bcdnode0
        COMMAND rm -rf data/bcdnode1
        COMMAND rm -rf data/bcdnode2
        COMMAND mkdir -p data/bcdnode0
        COMMAND mkdir -p data/bcdnode1
        COMMAND mkdir -p data/bcdnode2
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
# copy resulting plugins into source directory
add_custom_target(shadow-ISP-bitcoin-copy-plugin ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Bitcoin_0_19_1dev::Plugin> ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(shadow-ISP-bitcoin-copy-plugin Bitcoin_0_19_1dev::Plugin)


add_shadow_plugin(shadow-ISP-bitcoin-client-plugin one_node_setmine.c)
target_link_libraries(shadow-ISP-bitcoin-client-plugin
        CURL_7_70_0::CURL)
# copy resulting plugins into source directory
add_custom_command(TARGET shadow-ISP-bitcoin-client-plugin
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:shadow-ISP-bitcoin-client-plugin> ${CMAKE_CURRENT_SOURCE_DIR})


add_test(NAME shadow-ISP-bitcoin
        COMMAND shadow test-bitcoin.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set_tests_properties(shadow-ISP-bitcoin PROPERTIES TIMEOUT 60)

### add test for bitcoin library & emulating benign nodes
add_shadow_plugin(shadow-ISP-bitcoin-emulated-benigns emulated-benigns.cpp)
target_link_libraries(shadow-ISP-bitcoin-emulated-benigns Libev_4_33::Libev_static)
target_link_libraries(shadow-ISP-bitcoin-emulated-benigns SHADOW_INTERFACE)
target_link_libraries(shadow-ISP-bitcoin-emulated-benigns Bitcoin_0_19_1dev::Plugin)
# copy resulting plugins into source directory
add_custom_command(TARGET shadow-ISP-bitcoin-emulated-benigns
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:shadow-ISP-bitcoin-emulated-benigns> ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME shadow-ISP-emulated-benigns
        COMMAND shadow test-bitcoinlib-benigns.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set_tests_properties(shadow-ISP-emulated-benigns PROPERTIES TIMEOUT 60)


### realistic bitcoin emulation for EREBUS attack
add_shadow_plugin(shadow-ISP-bitcoin-emulated-benigns-realistic emulated-benigns-realistic.cpp)
target_link_libraries(shadow-ISP-bitcoin-emulated-benigns-realistic Libev_4_33::Libev_static)
target_link_libraries(shadow-ISP-bitcoin-emulated-benigns-realistic SHADOW_INTERFACE)
target_link_libraries(shadow-ISP-bitcoin-emulated-benigns-realistic Bitcoin_0_19_1dev::Plugin)
# copy resulting plugins into source directory
add_custom_command(TARGET shadow-ISP-bitcoin-emulated-benigns-realistic
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:shadow-ISP-bitcoin-emulated-benigns-realistic> ${CMAKE_CURRENT_SOURCE_DIR})


###
add_executable(test-template test-template.cpp)
target_link_libraries(test-template Libev_4_33::Libev_static)
target_link_libraries(test-template SHADOW_INTERFACE)
target_link_libraries(test-template Bitcoin_0_19_1dev::Plugin)
target_link_libraries(test-template -lboost_system)