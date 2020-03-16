
add_shadow_plugin(NODE_BL_TEST node_bl_test.cpp)
target_link_libraries(NODE_BL_TEST NEWBLEEP)
target_link_libraries(NODE_BL_TEST BLEEP)

install(TARGETS NODE_BL_TEST DESTINATION plugins)

add_test(NAME BLEEP-regtest-simpleconnect
  COMMAND python test.py --noserver test-simpleconnect/simple-connect.xml
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/..")
