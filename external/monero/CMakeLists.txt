
cmake_minimum_required(VERSION 2.8)

if (NOT CMAKE_INSTALL_PREFIX OR
        CMAKE_INSTALL_PREFIX STREQUAL "/usr/local")
    set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/../../Install")
endif()

add_custom_target(monero_0.16.0 ALL
        DEPENDS ${CMAKE_INSTALL_PREFIX}/plugins/libmonerod.so
        )

add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/dev_0.16.0/build/debug/external/libunbound.so
        COMMAND patch -p1 < unbound_spin.lock.patch
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/dev_0.16.0/external/unbound
        )

add_custom_command(OUTPUT ${CMAKE_INSTALL_PREFIX}/plugins/libmonerod.so
        COMMAND git submodule update --init
        COMMAND make -j6 debug
        #COMMAND cp build/debug/src/daemon/libmonerod.so ${CMAKE_INSTALL_PREFIX}/plugins/libmonerod.so
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/dev_0.16.0
        )

add_library(Monero_0.16.0::Plugin SHARED IMPORTED GLOBAL)
add_dependencies(Monero_0.16.0::Plugin monero_0.16.0)
set_target_properties(Monero_0.16.0::Plugin PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES "C"
        IMPORTED_LOCATION ${CMAKE_INSTALL_PREFIX}/plugins/libmonerod.so
        )

