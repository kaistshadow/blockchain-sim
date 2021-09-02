# copy bitcoind plugin
add_custom_target(16_BITCOIND_copy-plugin ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Bitcoin_0_19_1dev::Plugin> ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(16_BITCOIND_copy-plugin Bitcoin_0_19_1dev::Plugin)

# copy TPSTESTER plugin
add_custom_target(16_TPSTESTER-plugin ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BITCOINTPS_TESTER> ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(16_TPSTESTER-plugin BITCOINTPS_TESTER)

# copy Miner node rpc plugin
add_custom_target(16_MinerNodeRPC-plugin ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:MinerNodeRPC::Plugin> ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME TPSTEST-connection
  COMMAND python3 tps.py
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
