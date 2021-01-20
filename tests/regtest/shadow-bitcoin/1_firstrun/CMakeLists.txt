## required for independent build
cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)


## enable CTEST
enable_testing()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Install/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../shadow/cmake")
include(ShadowTools) # load ShadowTools Module


find_package(Bitcoin_0_19_1nw REQUIRED)

# message("BITCOIN_0_19_1NW_FOUND=${BITCOIN_0_19_1NW_FOUND}")

add_custom_target(shadow-bitcoin-firstrun-prepare ALL
  COMMAND mkdir -p data/bcdnode1
  COMMAND mkdir -p data/bcdnode2
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(copy-bitcoin-plugin ALL
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Bitcoin_0_19_1nw::Plugin> ${CMAKE_CURRENT_SOURCE_DIR})
      
add_test(NAME shadow-bitcoin-firstrun
  COMMAND shadow example.xml
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME shadow-bitcoin-firstrun_log_check
  COMMAND python test.py --bitcoin
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../)

