# register the test
add_test(NAME BLEEP-example
  COMMAND python test-example.py
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
# add_test(NAME BLEEP-example
#   COMMAND ${SHADOW_ROOT}/bin/shadow -d example-datadir ${CMAKE_CURRENT_SOURCE_DIR}/example.xml
#   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})


add_test(NAME BLEEP-simple-consensus
  COMMAND python test-simple-consensus.py
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
# add_test(NAME BLEEP-simple-consensus
#   COMMAND ${SHADOW_ROOT}/bin/shadow -d simple-consensus-datadir ${CMAKE_CURRENT_SOURCE_DIR}/simple_consensus.xml
#   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

