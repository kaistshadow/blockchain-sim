cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

cmake_policy(SET CMP0057 NEW)


macro(find_package)
  set(as_subproject OpenSSL_1_1_0h Bitcoin_0_19_1 Bitcoin_0_19_1nw Bitcoin_0_19_1dev CURL_7_70_0  Libev_4_33 LITECOIN__DEV_)
  if (NOT "${ARGV0}" IN_LIST as_subproject )
    _find_package(${ARGV})
  endif()
endmacro()

message("--------------------------------------------------------------------------------------")
message("                           Start default dependencies build")
message("--------------------------------------------------------------------------------------")
add_subdirectory(openssl)
add_subdirectory(bdb)
add_subdirectory(curl)
add_subdirectory(libev)


if (ALL_OPT STREQUAL ON)
    message("--------------------------------------------------------------------------------------")
    message("                           Start All build")
    message("--------------------------------------------------------------------------------------")
    add_subdirectory(bitcoin)
elseif(BITCOIN_OPT STREQUAL ON)
  message("--------------------------------------------------------------------------------------")
  message("                           Start Bitcoin build")
  message("--------------------------------------------------------------------------------------")
  add_subdirectory(bitcoin)

elseif(MONERO_OPT STREQUAL ON)
    message("--------------------------------------------------------------------------------------")
    message("                           Start Monero build")
    message("--------------------------------------------------------------------------------------")
    add_subdirectory(monero)

elseif (LITECOIN_OPT STREQUAL ON)
    message("--------------------------------------------------------------------------------------")
    message("                           Start Litecoin build")
    message("--------------------------------------------------------------------------------------")
    add_subdirectory(litecoin)

elseif (GIT_OPT STREQUAL ON)
    message("--------------------------------------------------------------------------------------")
    message("                           Git All build")
    message("--------------------------------------------------------------------------------------")
    add_subdirectory(bitcoin)

elseif(ALL_OPT STREQUAL ON)
  message("--------------------------------------------------------------------------------------")
  message("                           Start All build")
  message("--------------------------------------------------------------------------------------")
  add_subdirectory(bitcoin)
  add_subdirectory(litecoin)

endif(ALL_OPT STREQUAL ON)
