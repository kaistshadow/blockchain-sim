#prepare bitcoin experiment
add_custom_target(sybil-API-bitcoin-prepare ALL
        COMMAND rm -rf data/bcdnode0
        COMMAND mkdir -p data/bcdnode0
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# compile bitcoin P2P tester
add_library(BitcoinSybilAdapter SHARED
        BitcoinNodeParams.h
        BitcoinNodePrimitives.cpp
        BitcoinNodePrimitives.h)
target_link_libraries(BitcoinSybilAdapter BLEEPsybil)
target_include_directories(BitcoinSybilAdapter
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR})

add_shadow_plugin(BITCOINP2P_TESTER)
target_link_libraries(BITCOINP2P_TESTER BLEEPsybil)
target_link_libraries(BITCOINP2P_TESTER BitcoinSybilAdapter)
target_link_libraries(BITCOINP2P_TESTER Bitcoin_0_19_1dev::Plugin)

target_sources(BITCOINP2P_TESTER
        PRIVATE
        tester.cpp)

# copy plugins into source directory
add_custom_target(sybil-API-bitcoin-copy-plugin ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Bitcoin_0_19_1dev::Plugin> ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(sybil-API-bitcoin-copy-plugin Bitcoin_0_19_1dev::Plugin)

add_custom_command(TARGET BITCOINP2P_TESTER
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BITCOINP2P_TESTER> ${CMAKE_CURRENT_SOURCE_DIR})
