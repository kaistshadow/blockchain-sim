# compile bitcoin P2P tester
add_library(BitcoinTPSAdapter_14 SHARED
        BitcoinNodePrimitives.cpp
        BitcoinNodePrimitives.h)
target_link_libraries(BitcoinTPSAdapter_14 BLEEPtps)
target_include_directories(BitcoinTPSAdapter_14
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR})

add_shadow_plugin(BITCOINTPS_TESTER_14)
target_link_libraries(BITCOINTPS_TESTER_14 BLEEPtps)
target_link_libraries(BITCOINTPS_TESTER_14 BitcoinTPSAdapter_14)
target_link_libraries(BITCOINTPS_TESTER_14 Bitcoin_0_19_1dev::Plugin)

target_sources(BITCOINTPS_TESTER_14
        PRIVATE
        tps_tester.cpp)

add_shadow_plugin(BITCOIN_MONITOR_14)
target_link_libraries(BITCOIN_MONITOR_14 BLEEPtps)
target_link_libraries(BITCOIN_MONITOR_14 BitcoinTPSAdapter_14)
target_link_libraries(BITCOIN_MONITOR_14 Bitcoin_0_19_1dev::Plugin)

target_sources(BITCOIN_MONITOR_14
        PRIVATE
        bitcoin_monitor.cpp)



# copy plugins into source directory
add_custom_target(14_monitorNode_blockReceive-bitcoin-copy-plugin_14 ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Bitcoin_0_19_1dev::Plugin> ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(14_monitorNode_blockReceive-bitcoin-copy-plugin_14 Bitcoin_0_19_1dev::Plugin)

add_custom_command(TARGET BITCOINTPS_TESTER_14
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BITCOINTPS_TESTER_14> ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_command(TARGET BITCOIN_MONITOR_14
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BITCOIN_MONITOR_14> ${CMAKE_CURRENT_SOURCE_DIR})