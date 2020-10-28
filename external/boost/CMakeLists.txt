cmake_minimum_required(VERSION 2.8)

if (NOT CMAKE_INSTALL_PREFIX OR
        CMAKE_INSTALL_PREFIX STREQUAL "/usr/local")
    set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/../../Install")
endif()

add_custom_target(boost ALL
    COMMAND ./bootstrap.sh && ./b2 headers && ./b2 -j 5
    COMMAND export BOOST_ROOT=~/boost_1_70_0
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/boost_1_70_0
    )
