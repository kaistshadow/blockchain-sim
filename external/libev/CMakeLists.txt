
if (NOT CMAKE_INSTALL_PREFIX OR
        CMAKE_INSTALL_PREFIX STREQUAL "/usr/local")
    set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/../../Install")
endif()

file(GLOB_RECURSE LIBEV_4.33_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/4.33 *.c *.h)

add_custom_target(libev_4.33 ALL
        DEPENDS ${CMAKE_INSTALL_PREFIX}/libev_4.33/lib/libev.so
        )

add_custom_command(OUTPUT ${CMAKE_INSTALL_PREFIX}/libev_4.33/lib/libev.so
        COMMAND ./configure --prefix=${CMAKE_INSTALL_PREFIX}/libev_4.33 --with-pic
        COMMAND make install
        DEPENDS ${LIBEV_4.33_SRCS}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/4.33
        )

# Define targets that can be used in same build-tree
add_library(Libev_4_33::Libev SHARED IMPORTED GLOBAL)
add_dependencies(Libev_4_33::Libev libev_4.33)
set_target_properties(Libev_4_33::Libev PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES "C"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/4.33"
        IMPORTED_LOCATION ${CMAKE_INSTALL_PREFIX}/libev_4.33/lib/libev.so)

add_library(Libev_4_33::Libev_static STATIC IMPORTED GLOBAL)
add_dependencies(Libev_4_33::Libev_static libev_4.33)
set_target_properties(Libev_4_33::Libev_static PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES "C"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/4.33"
        IMPORTED_LOCATION ${CMAKE_INSTALL_PREFIX}/libev_4.33/lib/libev.a)
