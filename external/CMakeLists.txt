cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

cmake_policy(SET CMP0057 NEW)

set(as_subproject OpenSSL_1_1_0h)
macro(find_package)
  message("ARG0=${ARGV0}")
  if (NOT "${ARGV0}" IN_LIST as_subproject )
    _find_package(${ARGV})
  endif()
endmacro()

add_subdirectory(bitcoin)
add_subdirectory(openssl)
add_subdirectory(curl)
# add_subdirectory(bdb)

