
cmake_minimum_required(VERSION 2.8)

if (NOT CMAKE_INSTALL_PREFIX OR
        CMAKE_INSTALL_PREFIX STREQUAL "/usr/local")
    set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/../../Install")
endif()

add_custom_target(ripple_dev ALL
    COMMAND python setup.py
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/rippled
        )
