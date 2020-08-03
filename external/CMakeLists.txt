cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

cmake_policy(SET CMP0057 NEW)


macro(find_package)
  set(as_subproject OpenSSL_1_1_0h Bitcoin_0_19_1 Bitcoin_0_19_1nw Bitcoin_0_19_1dev CURL_7_70_0 Geth_1_9_1dev)
  if (NOT "${ARGV0}" IN_LIST as_subproject )
    _find_package(${ARGV})
  endif()
endmacro()

add_subdirectory(bitcoin)
add_subdirectory(openssl)
add_subdirectory(curl)
add_subdirectory(bdb)
add_subdirectory(go-ethereum)
