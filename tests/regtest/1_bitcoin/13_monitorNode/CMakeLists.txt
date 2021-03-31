#prepare bitcoin experiment
add_custom_target(connection_client-prepare ALL
        COMMAND rm -rf data/bcdnode0
        COMMAND mkdir -p data/bcdnode0
        COMMAND rm -rf data/bcdnode1
        COMMAND mkdir -p data/bcdnode1
        COMMAND cp -r ../../../../testlibs/dump/difficulty_3/bcdnode0/* data/bcdnode0
        COMMAND cp -r ../../../../testlibs/dump/difficulty_3/bcdnode0/* data/bcdnode1
        COMMAND cp -r ../../../../testlibs/dump/difficulty_3/coinflip_hash.txt data/

        # copy rpc.so from BLEEPemul
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/BLEEPemul/emulation/rpc.so ${CMAKE_CURRENT_SOURCE_DIR}

        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# compile bitcoin P2P tester
add_library(BitcoinTPSAdapter2 SHARED
        BitcoinNodePrimitives.cpp
        BitcoinNodePrimitives.h)
target_link_libraries(BitcoinTPSAdapter2 BLEEPtps)
target_include_directories(BitcoinTPSAdapter2
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR})

add_shadow_plugin(BITCOINTPS_TESTER2)
target_link_libraries(BITCOINTPS_TESTER2 BLEEPtps)
target_link_libraries(BITCOINTPS_TESTER2 BitcoinTPSAdapter2)
target_link_libraries(BITCOINTPS_TESTER2 Bitcoin_0_19_1dev::Plugin)

target_sources(BITCOINTPS_TESTER2
        PRIVATE
        tps_tester.cpp)

add_shadow_plugin(BITCOIN_MONITOR2)
target_link_libraries(BITCOIN_MONITOR2 BLEEPtps)
target_link_libraries(BITCOIN_MONITOR2 BitcoinTPSAdapter2)
target_link_libraries(BITCOIN_MONITOR2 Bitcoin_0_19_1dev::Plugin)

target_sources(BITCOIN_MONITOR2
        PRIVATE
        bitcoin_monitor.cpp)



# copy plugins into source directory
add_custom_target(connection_client-copy-plugin ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Bitcoin_0_19_1dev::Plugin> ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(connection_client-copy-plugin Bitcoin_0_19_1dev::Plugin)

add_custom_command(TARGET BITCOINTPS_TESTER2
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BITCOINTPS_TESTER2> ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_command(TARGET BITCOIN_MONITOR2
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BITCOIN_MONITOR2> ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME monitor_test
  COMMAND python3 monitor.py
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)