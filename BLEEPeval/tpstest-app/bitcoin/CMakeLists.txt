if (NOT CMAKE_INSTALL_PREFIX OR
    CMAKE_INSTALL_PREFIX STREQUAL "/usr/local")
  set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/../../../Install")
endif()

#prepare bitcoin experiment

set(CMAKE_BUILD_TYPE Debug)

add_custom_target(tps-API-bitcoin-prepare ALL
        COMMAND sh clean_data.sh 2
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
# compile bitcoin P2P tester
add_library(BitcoinTPSAdapter SHARED
        BitcoinNodePrimitives.cpp
        BitcoinNodePrimitives.h)
target_link_libraries(BitcoinTPSAdapter BLEEPtps)
target_include_directories(BitcoinTPSAdapter
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR})

add_shadow_plugin(BITCOINTPS_TESTER)
target_link_libraries(BITCOINTPS_TESTER BLEEPtps)
target_link_libraries(BITCOINTPS_TESTER BitcoinTPSAdapter)
target_link_libraries(BITCOINTPS_TESTER Bitcoin_0_19_1dev::Plugin)

target_sources(BITCOINTPS_TESTER
        PRIVATE
        tps_tester.cpp)

add_custom_target(tps_test ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BITCOINTPS_TESTER> ${CMAKE_INSTALL_PREFIX}/plugins
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BITCOINTPS_TESTER> ${CMAKE_CURRENT_SOURCE_DIR}
        )

# Define targets that can be used in same build-tree
add_library(TPSTESTER::Plugin SHARED IMPORTED GLOBAL)
add_dependencies(TPSTESTER::Plugin bitcoin_0.19.1dev)
set_target_properties(TPSTESTER::Plugin PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES "C"
        IMPORTED_LOCATION ${CMAKE_INSTALL_PREFIX}/plugins/libBITCOINTPS_TESTER.so
        INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/src)
set_property(TARGET TPSTESTER::Plugin PROPERTY IMPORTED_NO_SONAME 1)


#build latency_tester.cpp
add_shadow_plugin(BITCOINLATENCY_TESTER)
target_link_libraries(BITCOINLATENCY_TESTER BLEEPtps)
target_link_libraries(BITCOINLATENCY_TESTER BitcoinTPSAdapter)
target_link_libraries(BITCOINLATENCY_TESTER Bitcoin_0_19_1dev::Plugin)

target_sources(BITCOINLATENCY_TESTER
        PRIVATE
        latency_tester.cpp)

add_custom_target(latency_test ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BITCOINLATENCY_TESTER> ${CMAKE_INSTALL_PREFIX}/plugins
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BITCOINLATENCY_TESTER> ${CMAKE_CURRENT_SOURCE_DIR}
        )



# copy plugin into source directory
add_custom_target(tps-API-bitcoin-copy-plugin ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Bitcoin_0_19_1dev::Plugin> ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND g++ -o minerNode_onlymining.so one_node_setmine.cpp ../../../testlibs/rpc_client.cpp -ljsoncpp -lcurl -lssl -shared -fPIC -Wl,-rpath=${CMAKE_CURRENT_SOURCE_DIR}/../../../Install/curl_7.70.0/lib -L${CMAKE_CURRENT_SOURCE_DIR}/../../../Install/curl_7.70.0/lib -g
        COMMAND ${CMAKE_COMMAND} -E copy minerNode_onlymining.so ${CMAKE_INSTALL_PREFIX}/plugins/minerNode_onlymining.so
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})


add_dependencies(tps-API-bitcoin-copy-plugin Bitcoin_0_19_1dev::Plugin)