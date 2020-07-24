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

# Define targets that can be used in same build-tree
add_library(OpenSSL_1_1_0h::Crypto SHARED IMPORTED GLOBAL)
add_dependencies(OpenSSL_1_1_0h::Crypto openssl_1.1.0h)
set_target_properties(OpenSSL_1_1_0h::Crypto
  PROPERTIES
  IMPORTED_LOCATION ${CMAKE_INSTALL_PREFIX}/openssl_1.1.0h/lib/libcrypto.so
  IMPORTED_LINK_INTERFACE_LANGUAGES "C"
  INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/include)
  # INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_INSTALL_PREFIX}/openssl_1.1.0h/include)


add_library(OpenSSL_1_1_0h::SSL SHARED IMPORTED GLOBAL)
add_dependencies(OpenSSL_1_1_0h::SSL openssl_1.1.0h)
set_target_properties(OpenSSL_1_1_0h::SSL
  PROPERTIES
  IMPORTED_LOCATION ${CMAKE_INSTALL_PREFIX}/openssl_1.1.0h/lib/libssl.so
  IMPORTED_LINK_INTERFACE_LANGUAGES "C"
  # INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_INSTALL_PREFIX}/openssl_1.1.0h/include
  INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/include
  INTERFACE_LINK_LIBRARIES OpenSSL_1_1_0h::Crypto)
