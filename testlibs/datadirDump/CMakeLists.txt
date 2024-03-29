
## required for independent build
cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)

## enable CTEST
enable_testing()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../Install/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../shadow/cmake")

find_package(Bitcoin_0_19_1dev REQUIRED)

add_custom_target(emulation-prepare2 ALL
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(emulation-copy-plugin2 ALL
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Bitcoin_0_19_1dev::Plugin> ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(emulation-one-node-mine-plugin2 ALL
        COMMAND g++ -o rpc.so one_node_setmine.cpp ../../testlibs/rpc_client.cpp -ljsoncpp -lcurl -lssl -shared -fPIC -Wl,-rpath=${CMAKE_CURRENT_SOURCE_DIR}/../../Install/curl_7.70.0/lib -L${CMAKE_CURRENT_SOURCE_DIR}/../../Install/curl_7.70.0/lib -g
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
