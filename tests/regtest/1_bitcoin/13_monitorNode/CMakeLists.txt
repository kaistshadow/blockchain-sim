# copy bitcoind plugin
add_custom_target(13_BITCOIND_copy-plugin ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Bitcoin_0_19_1dev::Plugin> ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(13_BITCOIND_copy-plugin Bitcoin_0_19_1dev::Plugin)

# copy Monitor Node plugin
add_custom_target(13_MonitorNode-plugin ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:MonitorNode::Plugin> ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(13_MonitorNode-plugin MonitorNode::Plugin)

# copy Miner node rpc plugin
add_custom_target(13_MinerNodeRPC-plugin ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:MinerNodeRPC::Plugin> ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(13_MinerNodeRPC-plugin MinerNodeRPC::Plugin)

add_test(NAME MonitorNode-connection
  COMMAND python3 monitor.py
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)