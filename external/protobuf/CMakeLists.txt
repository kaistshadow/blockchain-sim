cmake_minimum_required(VERSION 2.8)

if (NOT CMAKE_INSTALL_PREFIX OR
        CMAKE_INSTALL_PREFIX STREQUAL "/usr/local")
    set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/../../Install")
endif()

# add_custom_command(
#         OUTPUT ${CMAKE_INSTALL_PREFIX}/Install/libprotobuf.so
#         COMMAND ./autogen.sh && ./configure && make -j 5
#         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/protobuf
#         )


add_custom_target(protobuf ALL
        COMMAND ./autogen.sh && ./configure && make -j 5
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/protobuf
   )
