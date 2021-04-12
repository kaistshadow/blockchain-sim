# copy bitcoind plugin
add_custom_target(TxGenerator_connection-copy-plugin ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Bitcoin_0_19_1dev::Plugin> ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(TxGenerator_connection-copy-plugin Bitcoin_0_19_1dev::Plugin)

# copy TPSTESTER plugin
add_custom_target(TPSTESTER-plugin ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:TPSTESTER::Plugin> ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(TPSTESTER-plugin TPSTESTER::Plugin)

# copy Monitor Node plugin
add_custom_target(MonitorNode-plugin ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:MonitorNode::Plugin> ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(MonitorNode-plugin MonitorNode::Plugin)

# copy Miner node rpc plugin
add_custom_target(MinerNodeRPC-plugin ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:MinerNodeRPC::Plugin> ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(MinerNodeRPC-plugin MinerNodeRPC::Plugin)