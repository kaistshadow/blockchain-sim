add_shadow_plugin(NODE_BL_TEST_TX_10 node_bl_test_tx.cpp)
install(TARGETS NODE_BL_TEST_TX_10 DESTINATION plugins)
target_link_libraries(NODE_BL_TEST_TX_10 NEWBLEEP)
target_link_libraries(NODE_BL_TEST_TX_10 BLEEP)

add_test(NAME BLEEP-gossip-10node_test
    COMMAND python test-broadcast.py --noserver 10node-txgossip.xml
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/")