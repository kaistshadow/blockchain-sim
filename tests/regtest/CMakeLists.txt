## ensure cmake version
cmake_minimum_required(VERSION 2.8)

## we use cmake files from shadow
if(EXISTS "${CMAKE_MODULE_PATH}")
    message(STATUS "CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")
else()
    ## we need to know where shadow repository is located
    if("${SHADOW_SRC_ROOT}" STREQUAL "")
        set(SHADOW_SRC_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../shadow")
    endif("${SHADOW_SRC_ROOT}" STREQUAL "")

    set(CMAKE_MODULE_PATH "${SHADOW_SRC_ROOT}/cmake")
    message(STATUS "CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")
endif()


include(ShadowTools)


add_shadow_plugin(NODE_BL_TEST BL2_peer_connectivity/node_bl_test.cpp)
install(TARGETS NODE_BL_TEST DESTINATION plugins)
target_link_libraries(NODE_BL_TEST NEWBLEEP)
target_link_libraries(NODE_BL_TEST BLEEP)

add_test(NAME BLEEP-regtest-simpleconnect
        COMMAND python test.py --noserver BL2_peer_connectivity/simple-connect.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(test-consensus) 
add_subdirectory(rust)
