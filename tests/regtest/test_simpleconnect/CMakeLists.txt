add_shadow_plugin(NODE_BL_TEST node_bl_test.cpp)
install(TARGETS NODE_BL_TEST DESTINATION plugins)
target_link_libraries(NODE_BL_TEST NEWBLEEP)
target_link_libraries(NODE_BL_TEST BLEEP)
 
add_test(NAME BLEEP-regtest-simpleconnect
COMMAND python test.py --noserver test_simpleconnect/simple-connect.xml
WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/..")