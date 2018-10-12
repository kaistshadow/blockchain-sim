# register the test

add_test(NAME BLEEP-simple-consensus
  COMMAND python test-simple-consensus.py
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
# add_test(NAME BLEEP-simple-consensus
#   COMMAND ${SHADOW_ROOT}/bin/shadow -d simple-consensus-datadir ${CMAKE_CURRENT_SOURCE_DIR}/simple_consensus.xml
#   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})


add_test(NAME BLEEP-nonblock-io
  COMMAND python test-nonblock-io.py
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})


add_test(NAME BLEEP-injector-simple
  COMMAND python test-injector-simple.py
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME BLEEP-stellar-consensus
  COMMAND python test-stellar-consensus.py
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME BLEEP-pow-consensus
  COMMAND python test-pow-consensus.py
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME BLEEP-plumtree-broadcast
  COMMAND python test-plumtree-broadcast.py
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  
