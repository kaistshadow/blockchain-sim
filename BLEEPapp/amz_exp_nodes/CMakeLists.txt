## compile for shadow example
# add_shadow_plugin(NODE_CTXTEST_GEN node_ctxtest_gen.cpp)
# install(TARGETS NODE_CTXTEST_GEN DESTINATION plugins)
# target_link_libraries(NODE_CTXTEST_GEN BLEEP::BLEEP)

# add_shadow_plugin(NODE_CTXTEST_RECV node_ctxtest_recv.cpp)
# install(TARGETS NODE_CTXTEST_RECV DESTINATION plugins)
# target_link_libraries(NODE_CTXTEST_RECV BLEEP::BLEEP)

add_shadow_plugin(NODE_RTT node_rtt.cpp)
install(TARGETS NODE_RTT DESTINATION plugins)
target_link_libraries(NODE_RTT BLEEP::BLEEP)

add_shadow_plugin(NODE_RTT_POW node_rtt_pow.cpp)
install(TARGETS NODE_RTT_POW DESTINATION plugins)
target_link_libraries(NODE_RTT_POW BLEEP::BLEEP)

add_shadow_plugin(NODE_RTT_POW_RAN node_rtt_pow_ran.cpp)
install(TARGETS NODE_RTT_POW_RAN DESTINATION plugins)
target_link_libraries(NODE_RTT_POW_RAN BLEEP::BLEEP)

## compile for local executable
# add_executable(LOCAL_NODE_CTXTEST_GEN node_ctxtest_gen.cpp)
# target_link_libraries(LOCAL_NODE_CTXTEST_GEN BLEEP::BLEEP)

# add_executable(LOCAL_NODE_CTXTEST_RECV node_ctxtest_recv.cpp)
# target_link_libraries(LOCAL_NODE_CTXTEST_RECV BLEEP::BLEEP)

add_executable(LOCAL_NODE_RTT node_rtt.cpp)
target_link_libraries(LOCAL_NODE_RTT BLEEP::BLEEP)

add_executable(LOCAL_NODE_RTT_POW node_rtt_pow.cpp)
target_link_libraries(LOCAL_NODE_RTT_POW BLEEP::BLEEP)

add_executable(LOCAL_NODE_RTT_POW_RAN node_rtt_pow_ran.cpp)
target_link_libraries(LOCAL_NODE_RTT_POW_RAN BLEEP::BLEEP)

