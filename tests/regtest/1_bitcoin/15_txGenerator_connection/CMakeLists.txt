# copy bitcoind plugin
add_custom_target(15_TxGenerator_connection-copy-plugin ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Bitcoin_0_19_1dev::Plugin> ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(15_TxGenerator_connection-copy-plugin Bitcoin_0_19_1dev::Plugin)

# copy TPSTESTER plugin
add_custom_target(15_TPSTESTER-plugin ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:TPSTESTER::Plugin> ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(15_TPSTESTER-plugin TPSTESTER::Plugin)

# copy Monitor Node plugin
add_custom_target(15_MonitorNode-plugin ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:MonitorNode::Plugin> ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(15_MonitorNode-plugin MonitorNode::Plugin)

# copy Miner node rpc plugin
add_custom_target(15_MinerNodeRPC-plugin ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:MinerNodeRPC::Plugin> ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(15_MinerNodeRPC-plugin MinerNodeRPC::Plugin)

add_test(NAME TxGenrator-connection
  COMMAND python3 block_propagation.py
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)