## required for independent build
cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)

## enable CTEST
enable_testing()
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Install/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../shadow/cmake")
include(ShadowTools) # load ShadowTools Module


find_package(Bitcoin_0_19_1dev REQUIRED)

add_custom_target(shadow_xmlGenerator_test_prepare ALL
  COMMAND rm -rf data/bcdnode0
  COMMAND mkdir -p data/bcdnode0
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(shadow_xmlGenerator_test_prepare2 ALL
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Bitcoin_0_19_1dev::Plugin> ${CMAKE_CURRENT_SOURCE_DIR}
  )
 
add_dependencies(shadow_xmlGenerator_test_prepare2 Bitcoin_0_19_1dev::Plugin)

add_test(NAME shadow_xmlGenerator_test
      COMMAND python shadowCheck.py --node=bcdnode0 --args=-debug-testnet-datadir=data/bcdnode0-port=11110-rpcuser=a-rpcpassword=1234-rpcport=11111-rpcallowip=11.0.0.1/0-rpcbind=11.0.0.1
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set_tests_properties(shadow_xmlGenerator_test PROPERTIES TIMEOUT 60)