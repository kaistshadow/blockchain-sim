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

add_shadow_plugin(shadow-ISP-2-attacker
  ISP-attacker.cpp)
target_link_libraries(shadow-ISP-2-attacker Libev_4_33::Libev)
target_link_libraries(shadow-ISP-2-attacker SHADOW_INTERFACE)

# copy resulting plugins into source directory
add_custom_command(TARGET shadow-ISP-2-attacker
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:shadow-ISP-2-attacker> ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME shadow-ISP-2
        COMMAND shadow test-churn-in-client.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})


################################################## for victim
## victim
add_shadow_plugin(shadow-ISP-2-victim
        victim.cpp)
target_link_libraries(shadow-ISP-2-victim Libev_4_33::Libev)
# copy resulting plugins into source directory
add_custom_command(TARGET shadow-ISP-2-victim
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:shadow-ISP-2-victim> ${CMAKE_CURRENT_SOURCE_DIR})


