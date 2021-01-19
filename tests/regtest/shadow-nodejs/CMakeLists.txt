## required for independent build
cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)

## required for independent build end

## include shadow
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../shadow/cmake") # Path for ShadowTool Module
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../Install/cmake")
include(ShadowTools) # load ShadowTools Module

## enable CTEST
enable_testing()


find_package(Nodejs REQUIRED)

add_custom_target(copy-nodejs-plugin ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Nodejs::Plugin> ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME shadow-nodejs-firstrun
        COMMAND shadow run-nodejs.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})



#add_shadow_plugin(NODEJS run-nodejs.cpp)
#target_link_libraries(NODEJS my_node.so)
#target_include_directories(NODEJS
#        /home/ilios/opt/nodejs/include
#        /home/ilios/opt/nodejs/include/node)

# copy resulting plugins into source directory
#add_custom_command(TARGET NODEJS
#        POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:NODEJS> ${CMAKE_CURRENT_SOURCE_DIR})

