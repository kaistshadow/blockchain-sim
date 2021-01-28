## ensure cmake version
cmake_minimum_required(VERSION 2.8)

## we use cmake files from shadow
if(EXISTS "${CMAKE_MODULE_PATH}")
    message(STATUS "CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")
else()
    ## we need to know where shadow repository is located
    if("${SHADOW_SRC_ROOT}" STREQUAL "")
        set(SHADOW_SRC_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../shadow")
    endif("${SHADOW_SRC_ROOT}" STREQUAL "")

    set(CMAKE_MODULE_PATH "${SHADOW_SRC_ROOT}/cmake")
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../Install/cmake")
    message(STATUS "CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")
endif()


include(ShadowTools)

if(ZCASH_OPT STREQUAL ON)
  message("------------------------------------------------------------------------------")
  message("                           Zcash build start")
  message("------------------------------------------------------------------------------")
  add_subdirectory(shadow-zcash)

elseif(BITCOIN_OPT STREQUAL ON)
  message("------------------------------------------------------------------------------")
  message("                           Bitcoin build start")
  message("------------------------------------------------------------------------------")
  add_subdirectory(shadow-bitcoin)
#  add_subdirectory(shadow-ISP)

elseif(ALL_OPT STREQUAL ON)
  message("------------------------------------------------------------------------------")
  message("                           All build start")
  message("------------------------------------------------------------------------------")
  add_subdirectory(shadow-bitcoin)
  add_subdirectory(shadow-zcash)
  add_subdirectory(BLEEP-simpleconnect)
  add_subdirectory(BLEEP-gossip-10node)
  add_subdirectory(BLEEP-POW-consensus)
  add_subdirectory(shadow-syscalls)
  add_subdirectory(shadow-dependencies)
  add_subdirectory(shadow_storage_mgmt_test)
  # add_subdirectory(shadow-ISP)

 elseif(GIT_OPT STREQUAL ON)
  message("------------------------------------------------------------------------------")
  message("                           Git build start")
  message("------------------------------------------------------------------------------")
  add_subdirectory(shadow-bitcoin)
  add_subdirectory(shadow-dependencies)
  add_subdirectory(BLEEP-simpleconnect)
  add_subdirectory(BLEEP-gossip-10node)
  add_subdirectory(BLEEP-POW-consensus)
  add_subdirectory(shadow-syscalls)
  # add_subdirectory(shadow-ISP)
  ### currently, eosio induces problem for git Action tests.
  ### Until the complete tests are done, let's exclude following external modules from git all build


else(ZCASH_OPT STREQUAL ON)
  message("------------------------------------------------------------------------------")
  message("                           default (defalut = Bitcoin)")
  message("------------------------------------------------------------------------------")
  add_subdirectory(BLEEP-simpleconnect)
  add_subdirectory(BLEEP-gossip-10node)
  add_subdirectory(BLEEP-POW-consensus)
  add_subdirectory(shadow-dependencies)
  add_subdirectory(native-bintest)
  add_subdirectory(shadow-bitcoin)
  add_subdirectory(shadow-syscalls)
  # add_subdirectory(shadow-ISP)
endif(ZCASH_OPT STREQUAL ON)
