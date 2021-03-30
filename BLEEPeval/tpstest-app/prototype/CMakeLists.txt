#prepare bitcoin experiment
add_custom_target(tps-API-bitcoin-prepare ALL
        COMMAND rm -rf data/bcdnode0
        COMMAND mkdir -p data/bcdnode0
        COMMAND rm -rf data/bcdnode1
        COMMAND mkdir -p data/bcdnode1
        COMMAND cp -r ../../../testlibs/dump/difficulty_3/bcdnode0/* data/bcdnode0
        COMMAND cp -r ../../../testlibs/dump/difficulty_3/bcdnode0/* data/bcdnode1
        COMMAND cp -r ../../../testlibs/dump/difficulty_3/coinflip_hash.txt data/

        # copy rpc.so from BLEEPemul
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/BLEEPemul/emulation/rpc.so ${CMAKE_CURRENT_SOURCE_DIR}

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

add_shadow_plugin(BITCOIN_MONITOR)
target_link_libraries(BITCOIN_MONITOR BLEEPtps)
target_link_libraries(BITCOIN_MONITOR BitcoinTPSAdapter)
target_link_libraries(BITCOIN_MONITOR Bitcoin_0_19_1dev::Plugin)

target_sources(BITCOIN_MONITOR
        PRIVATE
        bitcoin_monitor.cpp)



# copy plugins into source directory
add_custom_target(tps-API-bitcoin-copy-plugin ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Bitcoin_0_19_1dev::Plugin> ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(tps-API-bitcoin-copy-plugin Bitcoin_0_19_1dev::Plugin)

add_custom_command(TARGET BITCOINTPS_TESTER
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BITCOINTPS_TESTER> ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_command(TARGET BITCOIN_MONITOR
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BITCOIN_MONITOR> ${CMAKE_CURRENT_SOURCE_DIR})
