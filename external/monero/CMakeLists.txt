cmake_minimum_required(VERSION 2.8)
if (NOT CMAKE_INSTALL_PREFIX OR
        CMAKE_INSTALL_PREFIX STREQUAL "/usr/local")
    set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/../../Install")
endif()

add_custom_target(monero_0.16.0 ALL
  DEPENDS ${CMAKE_INSTALL_PREFIX}/plugins/libmonerod.so
)

add_custom_command(OUTPUT ${CMAKE_INSTALL_PREFIX}/plugins/libmonerod.so
	COMMAND git submodule update --init
        COMMAND make -j6 debug 
        #COMMAND cp build/debug/src/daemon/libmonerod.so ${CMAKE_INSTALL_PREFIX}/plugins/libmonerod.so
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/dev0.16.0
        )
