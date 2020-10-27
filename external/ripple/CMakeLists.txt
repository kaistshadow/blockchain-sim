
cmake_minimum_required(VERSION 2.8)

if (NOT CMAKE_INSTALL_PREFIX OR
        CMAKE_INSTALL_PREFIX STREQUAL "/usr/local")
    set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/../../Install")
endif()

add_custom_target(ripple_dev ALL
        DEPENDS ${CMAKE_INSTALL_PREFIX}/plugins/rippled
        )

add_custom_command(OUTPUT ${CMAKE_INSTALL_PREFIX}/plugins/rippled

        COMMAND python setup.py
                        #COMMAND cp build/debug/src/daemon/rippled ${CMAKE_INSTALL_PREFIX}/plugins/rippled
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/dev_ripple
        )

add_library(ripple_dev::Plugin SHARED IMPORTED GLOBAL)
add_dependencies(ripple_dev::Plugin ripple_dev)
set_target_properties(ripple_dev::Plugin PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES "C"
        IMPORTED_LOCATION ${CMAKE_INSTALL_PREFIX}/plugins/rippled
        )

