# copy bitcoind plugin
add_custom_target(17_BITCOIND_copy-plugin ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Bitcoin_0_19_1dev::Plugin> ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(17_BITCOIND_copy-plugin Bitcoin_0_19_1dev::Plugin)

# copy TPSTESTER plugin
add_custom_target(17_LatencyTESTER-plugin ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BITCOINLATENCY_TESTER> ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(17_LatencyTESTER-plugin BITCOINTPS_TESTER)

# copy Miner node rpc plugin
add_custom_target(17_MinerNodeRPC-plugin ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:MinerNodeRPC::Plugin> ${CMAKE_CURRENT_SOURCE_DIR})

# add_test(NAME TPSTEST-connection
#   COMMAND python3 latency.py
#   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
# )
