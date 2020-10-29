
cmake_minimum_required(VERSION 2.8)

if (NOT CMAKE_INSTALL_PREFIX OR
        CMAKE_INSTALL_PREFIX STREQUAL "/usr/local")
    set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/../../Install")
endif()

add_custom_target(ripple_dev ALL
        DEPENDS ${CMAKE_INSTALL_PREFIX}/plugins/rippled
        )

add_custom_command(OUTPUT ${CMAKE_INSTALL_PREFIX}/plugins/rippled
        COMMAND rm -rf build
        COMMAND export BOOST_ROOT=~/boost_1_70_0
        COMMAND mkdir build && cd build && cmake .. && make -j 10
        COMMAND python setup.py
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/dev_ripple
        )



