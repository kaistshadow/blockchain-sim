
add_shadow_plugin(NODE_POW_RANDOM_TREE node_pow_random_tree.cpp)
install(TARGETS NODE_POW_RANDOM_TREE DESTINATION plugins)
target_link_libraries(NODE_POW_RANDOM_TREE BLEEP)

add_shadow_plugin(NODE_client client.cpp)
install(TARGETS NODE_client DESTINATION plugins)
target_link_libraries(NODE_client BLEEP)

add_test(NAME BLEEP-regtest-consensus-test
  COMMAND python test.py --noserver test-consensus/Consensus[pow-tree]-Gossip[SP]-200node-2sec.xml
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/..")
