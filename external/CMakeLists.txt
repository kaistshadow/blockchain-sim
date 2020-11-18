cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

cmake_policy(SET CMP0057 NEW)


macro(find_package)
        set(as_subproject OpenSSL_1_1_0h Bitcoin_0_19_1 Bitcoin_0_19_1nw Bitcoin_0_19_1dev CURL_7_70_0 Geth_1_9_1dev Libev_4_33 Monero_0.16.0)
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

if(ZCASH_OPT STREQUAL ON)
  message("--------------------------------------------------------------------------------------")
  message("                           Start Zcash build")
  message("--------------------------------------------------------------------------------------")
  add_subdirectory(zcash)

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

elseif(ALL_OPT STREQUAL ON)
  message("--------------------------------------------------------------------------------------")
  message("                           Start All build")
  message("--------------------------------------------------------------------------------------")
  add_subdirectory(monero)
  add_subdirectory(zcash)
  add_subdirectory(bitcoin)

elseif(GIT_OPT STREQUAL ON)
  message("--------------------------------------------------------------------------------------")
  message("                           Git All build")
  message("--------------------------------------------------------------------------------------")
  add_subdirectory(zcash)
  add_subdirectory(bitcoin)
  
else(ZCASH_OPT STREQUAL ON)
  message("--------------------------------------------------------------------------------------")
  message("                           default")
  message("--------------------------------------------------------------------------------------")
  add_subdirectory(bitcoin)

endif(ZCASH_OPT STREQUAL ON)
  