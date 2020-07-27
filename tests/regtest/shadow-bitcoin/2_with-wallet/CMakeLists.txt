## required for independent build
cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)


# ExternalProject_Add_Step(bitcoin-with-wallet PATCH_FOR_PIC
#         COMMAND sed -i "s/PIE_FLAGS/PIC_FLAGS/g" Makefile.am
#         COMMAND sed -i "s/PIE_FLAGS/PIC_FLAGS/g" Makefile.leveldb.include
#         COMMENT "Patch Makefiles in order to use fPIC instead of fPIE."
#         DEPENDEES patch
#         DEPENDERS build
#         WORKING_DIRECTORY <SOURCE_DIR>/src
#         )

# ExternalProject_Add_Step(bitcoin-with-wallet PATCH_FOR_SOURCE
# #        COMMAND git apply ${CMAKE_CURRENT_SOURCE_DIR}/../DisableSanityCheck.patch || echo "skip DisableSanityCheck.patch"
#         COMMAND git apply ${CMAKE_CURRENT_SOURCE_DIR}/../InfiniteLoop.patch || echo "skip InfiniteLoop.patch"
#         COMMENT "Patch bitcoin src to resolve infinite execution."
#         DEPENDEES patch
#         DEPENDERS build
#         WORKING_DIRECTORY <SOURCE_DIR>
#         )

## enable CTEST
enable_testing()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Install/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../shadow/cmake")
include(ShadowTools) # load ShadowTools Module


find_package(Bitcoin_0_19_1 REQUIRED)

add_custom_target(shadow-bitcoin-with-wallet-prepare ALL
  COMMAND rm -rf data/bcdnode1
  COMMAND rm -rf data/bcdnode2
  COMMAND mkdir -p data/bcdnode1
  COMMAND mkdir -p data/bcdnode2
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(shadow-bitcoin-with-wallet-copy-plugin ALL
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Bitcoin_0_19_1::Plugin> ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME shadow-bitcoin-with-wallet
        COMMAND shadow example.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME shadow-bitcoin-with-wallet-check-pwrite-bug
        COMMAND shadow example-reproduce-logerr.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set_tests_properties(shadow-bitcoin-with-wallet PROPERTIES DEPENDS shadow-bitcoin-with-wallet-prepare)
set_tests_properties(shadow-bitcoin-with-wallet PROPERTIES TIMEOUT 60)
