## required for independent build
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
if (NOT TARGET BLEEP) # check whether it's independent build
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Install/cmake") # Path for BLEEP Module
    find_package(BLEEP REQUIRED) # load BLEEP Module
endif()
## required for independent build end

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Install/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../shadow/cmake") # Path for ShadowTool Module

find_package(Libev_4_33 REQUIRED) # libev4.33 is required because BLEEP uses libev4.33..  TODO: export libev4.33 when the BLEEP is exported?
include(ShadowTools) # load ShadowTools Module

## enable CTEST
enable_testing()


add_shadow_plugin(SIMPLE_SERVER simple-server.cpp)
target_link_libraries(SIMPLE_SERVER NEWBLEEP)
target_link_libraries(SIMPLE_SERVER BLEEP)

# copy resulting plugins into source directory
add_custom_command(TARGET SIMPLE_SERVER
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:SIMPLE_SERVER> ${CMAKE_CURRENT_SOURCE_DIR})


add_test(NAME regtest-sybil-simpleserver
        COMMAND shadow simple-server.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})


################################################### for sender
### sender
#add_shadow_plugin(shadow-ISP-churn-in-sender
#        sender.cpp)
#target_link_libraries(shadow-ISP-churn-in-sender Libev_4_33::Libev)
## copy resulting plugins into source directory
#add_custom_command(TARGET shadow-ISP-churn-in-sender
#        POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:shadow-ISP-churn-in-sender> ${CMAKE_CURRENT_SOURCE_DIR})
#
#
