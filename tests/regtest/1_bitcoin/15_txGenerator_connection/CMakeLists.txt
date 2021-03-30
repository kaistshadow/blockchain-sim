#prepare bitcoin experiment
add_custom_target(TxGenerator_connection-prepare ALL
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
add_library(BitcoinTPSAdapter_TxGenerator SHARED
        BitcoinNodePrimitives.cpp
        BitcoinNodePrimitives.h)
target_link_libraries(BitcoinTPSAdapter_TxGenerator BLEEPtps)
target_include_directories(BitcoinTPSAdapter_TxGenerator
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR})

add_shadow_plugin(TxGenerator_TESTER)
target_link_libraries(TxGenerator_TESTER BLEEPtps)
target_link_libraries(TxGenerator_TESTER BitcoinTPSAdapter_TxGenerator)
target_link_libraries(TxGenerator_TESTER Bitcoin_0_19_1dev::Plugin)

target_sources(TxGenerator_TESTER
        PRIVATE
        tps_tester.cpp)

add_shadow_plugin(BITCOIN_MONITER_TEST)
target_link_libraries(BITCOIN_MONITER_TEST BLEEPtps)
target_link_libraries(BITCOIN_MONITER_TEST BitcoinTPSAdapter_TxGenerator)
target_link_libraries(BITCOIN_MONITER_TEST Bitcoin_0_19_1dev::Plugin)

target_sources(BITCOIN_MONITER_TEST
        PRIVATE
        bitcoin_monitor.cpp)



# copy plugins into source directory
add_custom_target(TxGenerator_connection-copy-plugin ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Bitcoin_0_19_1dev::Plugin> ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(TxGenerator_connection-copy-plugin Bitcoin_0_19_1dev::Plugin)

add_custom_command(TARGET TxGenerator_TESTER
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:TxGenerator_TESTER> ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_command(TARGET BITCOIN_MONITER_TEST
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BITCOIN_MONITER_TEST> ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME txGenerator_connection_test
  COMMAND python3 txGenerator.py
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)