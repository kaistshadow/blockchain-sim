## required for independent build
cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)

if (NOT CMAKE_INSTALL_PREFIX OR
    CMAKE_INSTALL_PREFIX STREQUAL "/usr/local")
  set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/../../../Install")
endif()


file(GLOB_RECURSE SSL_1.1.0H_SRCS ${CMAKE_CURRENT_SOURCE_DIR} *.c *.h)

add_custom_target(openssl_1.1.0h ALL
  DEPENDS ${CMAKE_INSTALL_PREFIX}/openssl_1.1.0h/lib/libssl.so
)

add_custom_command(OUTPUT ${CMAKE_INSTALL_PREFIX}/openssl_1.1.0h/lib/libssl.so
  COMMAND ./config --prefix=${CMAKE_INSTALL_PREFIX}/openssl_1.1.0h shared threads enable-ec_nistp_64_gcc_128 -fPIC
  COMMAND make depend
  COMMAND make -j 4
  COMMAND make install_sw    # TODO : 'install' as separate target or command?
  DEPENDS ${SSL_1.1.0H_SRCS} # for re-compilation when the source is modified
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

