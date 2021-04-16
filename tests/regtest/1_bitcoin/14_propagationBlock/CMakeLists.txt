# copy bitcoind plugin
add_custom_target(14_TxGenerator_connection-copy-plugin ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Bitcoin_0_19_1dev::Plugin> ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(14_TxGenerator_connection-copy-plugin Bitcoin_0_19_1dev::Plugin)

# copy TPSTESTER plugin
add_custom_target(14_TPSTESTER-plugin ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BITCOINTPS_TESTER> ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(14_TPSTESTER-plugin BITCOINTPS_TESTER)

# copy Monitor Node plugin
add_custom_target(14_MonitorNode-plugin ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BITCOIN_MONITOR> ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(14_MonitorNode-plugin BITCOIN_MONITOR)

# # # copy Miner node rpc plugin
add_custom_target(14_MinerNodeRPC-plugin ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:MinerNodeRPC::Plugin> ${CMAKE_CURRENT_SOURCE_DIR})
# add_dependencies(14_MinerNodeRPC-Plugin MinerNodeRPC::Plugin)

add_test(NAME MonitorNode-blockPropagation
  COMMAND python3 block_propagation.py
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
set_tests_properties(MonitorNode-blockPropagation PROPERTIES TIMEOUT 150) 
