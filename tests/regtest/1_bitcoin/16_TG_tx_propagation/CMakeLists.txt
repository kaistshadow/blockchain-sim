# compile bitcoin P2P tester
add_library(BitcoinTPSAdapter_16 SHARED
        BitcoinNodePrimitives.cpp
        BitcoinNodePrimitives.h)
target_link_libraries(BitcoinTPSAdapter_16 BLEEPtps)
target_include_directories(BitcoinTPSAdapter_16
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR})

add_shadow_plugin(BITCOINTPS_TESTER_16)
target_link_libraries(BITCOINTPS_TESTER_16 BLEEPtps)
target_link_libraries(BITCOINTPS_TESTER_16 BitcoinTPSAdapter_16)
target_link_libraries(BITCOINTPS_TESTER_16 Bitcoin_0_19_1dev::Plugin)

target_sources(BITCOINTPS_TESTER_16
        PRIVATE
        tps_tester.cpp)

add_shadow_plugin(BITCOIN_MONITOR_16)
target_link_libraries(BITCOIN_MONITOR_16 BLEEPtps)
target_link_libraries(BITCOIN_MONITOR_16 BitcoinTPSAdapter_16)
target_link_libraries(BITCOIN_MONITOR_16 Bitcoin_0_19_1dev::Plugin)

target_sources(BITCOIN_MONITOR_16
        PRIVATE
        bitcoin_monitor.cpp)



# copy plugins into source directory
add_custom_target(16_txGen_txPropagation-bitcoin-copy-plugin_16 ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Bitcoin_0_19_1dev::Plugin> ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(16_txGen_txPropagation-bitcoin-copy-plugin_16 Bitcoin_0_19_1dev::Plugin)

add_custom_command(TARGET BITCOINTPS_TESTER_16
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BITCOINTPS_TESTER_16> ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_command(TARGET BITCOIN_MONITOR_16
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BITCOIN_MONITOR_14> ${CMAKE_CURRENT_SOURCE_DIR})