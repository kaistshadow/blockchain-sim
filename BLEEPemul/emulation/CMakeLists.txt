if (NOT CMAKE_INSTALL_PREFIX OR
CMAKE_INSTALL_PREFIX STREQUAL "/usr/local")
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/../../../Install")
endif()
## required for independent build
cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)

## enable CTEST
enable_testing()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../Install/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../shadow/cmake")

find_package(Bitcoin_0_19_1dev REQUIRED)

add_custom_target(emulation-prepare ALL
  COMMAND mkdir -p data/
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(emulation-copy-plugin ALL
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Bitcoin_0_19_1dev::Plugin> ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(emulation-one-node-mine-plugin ALL
        COMMAND g++ -o minerNode_rpc.so one_node_setmine.cpp ../../testlibs/rpc_client.cpp -ljsoncpp -lcurl -lssl -shared -fPIC -Wl,-rpath=${CMAKE_CURRENT_SOURCE_DIR}/../../Install/curl_7.70.0/lib -L${CMAKE_CURRENT_SOURCE_DIR}/../../Install/curl_7.70.0/lib -g
        COMMAND ${CMAKE_COMMAND} -E copy minerNode_rpc.so ${CMAKE_INSTALL_PREFIX}/plugins/minerNode_rpc.so
        COMMAND g++ -o transaction.so transaction.cpp  ../../testlibs/rpc_client.cpp -ljsoncpp -lcurl -lssl -shared -fPIC -Wl,-rpath=${CMAKE_CURRENT_SOURCE_DIR}/../../Install/curl_7.70.0/lib -L${CMAKE_CURRENT_SOURCE_DIR}/../../Install/curl_7.70.0/lib -g
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# Define targets that can be used in same build-tree
add_library(MinerNodeRPC::Plugin SHARED IMPORTED GLOBAL)
add_dependencies(MinerNodeRPC::Plugin bitcoin_0.19.1dev)
set_target_properties(MinerNodeRPC::Plugin PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES "C"
  IMPORTED_LOCATION ${CMAKE_INSTALL_PREFIX}/plugins/minerNode_rpc.so
  INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/src)
set_property(TARGET MinerNodeRPC::Plugin PROPERTY IMPORTED_NO_SONAME 1)

find_package(shadow REQUIRED)
find_package(Libev_4_33 REQUIRED)
include(ShadowTools) # load ShadowTools Module
add_shadow_plugin(BLEEPlib-test-pow-node node-pow.cpp)
target_link_libraries(BLEEPlib-test-pow-node BLEEP)
# copy resulting plugins into source directory
add_custom_command(TARGET BLEEPlib-test-pow-node
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BLEEPlib-test-pow-node> ${CMAKE_CURRENT_SOURCE_DIR})