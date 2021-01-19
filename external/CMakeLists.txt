cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

cmake_policy(SET CMP0057 NEW)


macro(find_package)
  set(as_subproject OpenSSL_1_1_0h Bitcoin_0_19_1 Bitcoin_0_19_1nw Bitcoin_0_19_1dev CURL_7_70_0 Geth_1_9_1dev Libev_4_33 Nodejs)
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

add_subdirectory(go-ethereum)

add_subdirectory(shadow_storage_manager)

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

elseif(LITECOIN_OPT STREQUAL ON)
  message("--------------------------------------------------------------------------------------")
  message("                           Start Litecoin build")
  message("--------------------------------------------------------------------------------------")
  add_subdirectory(litecoin)

elseif(NODEJS_OPT STREQUAL ON)  # TODO : implement NODEJS_OPT for setup script
  message("--------------------------------------------------------------------------------------")
  message("                           Start NODE.JS build")
  message("--------------------------------------------------------------------------------------")
  add_subdirectory(nodejs)

elseif(ALL_OPT STREQUAL ON)
  message("--------------------------------------------------------------------------------------")
  message("                           Start All build")
  message("--------------------------------------------------------------------------------------")
  add_subdirectory(zcash)
  add_subdirectory(bitcoin)
  add_subdirectory(litecoin)
  add_subdirectory(nodejs)

elseif(GIT_OPT STREQUAL ON)
  message("--------------------------------------------------------------------------------------")
  message("                           Git All build")
  message("--------------------------------------------------------------------------------------")
  add_subdirectory(bitcoin)
  add_subdirectory(nodejs)
  ### Until the complete tests are done, let's exclude following external modules from git all build
  #  add_subdirectory(zcash)
  #  add_subdirectory(litecoin)

else(ZCASH_OPT STREQUAL ON)
  message("--------------------------------------------------------------------------------------")
  message("                           default (defalut = NODEJS)")
  message("--------------------------------------------------------------------------------------")
  #add_subdirectory(bitcoin)
  add_subdirectory(nodejs) # for testing purpose. TODO: should be removed before merging to develop or master branch

endif(ZCASH_OPT STREQUAL ON)
