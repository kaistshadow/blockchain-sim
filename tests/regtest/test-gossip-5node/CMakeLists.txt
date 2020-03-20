add_shadow_plugin(NODE_BL_TEST_TX node_bl_test_tx.cpp)
install(TARGETS NODE_BL_TEST_TX DESTINATION plugins)
target_link_libraries(NODE_BL_TEST_TX NEWBLEEP)
target_link_libraries(NODE_BL_TEST_TX BLEEP)

add_test(NAME BLEEP-gossip-5node_test
    COMMAND python test.py --noserver test-gossip-5node/5node-txgossip.xml
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/..")