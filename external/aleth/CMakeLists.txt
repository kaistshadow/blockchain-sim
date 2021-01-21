cmake_minimum_required(VERSION 2.8)

if (NOT CMAKE_INSTALL_PREFIX OR
        CMAKE_INSTALL_PREFIX STREQUAL "/usr/local")
    set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/../../Install")
endif()

add_custom_target(aleth_dev ALL
    COMMAND git submodule update --init
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/aleth
    )

add_custom_target(aleth_dev2 ALL
    DEPENDS aleth_dev
    COMMAND mkdir -p build
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/aleth
    )    

add_custom_target(aleth_dev3 ALL
    DEPENDS aleth_dev2
    COMMAND cmake ..
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/aleth/build
    )  

add_custom_target(aleth_dev4 ALL
    DEPENDS aleth_dev3
    COMMAND make -j 6
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/aleth/build
    )  