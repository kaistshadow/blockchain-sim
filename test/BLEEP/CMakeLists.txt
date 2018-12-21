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

add_test(NAME BLEEP-pow-consensus-large
  COMMAND python test-pow-consensus.py --nodenum 10
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME BLEEP-centralized-broadcast
  COMMAND python test-centralized-broadcast.py
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  
add_test(NAME BLEEP-pow-emulated-mining
  COMMAND python test-pow-consensus-emulated-mining.py
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME BLEEP-asio
  COMMAND python test-asio.py
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME BLEEP-libev
  COMMAND python test-libev.py
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME BLEEP-centralized-async
  COMMAND python test-centralized-broadcast-async.py
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  

# add_test(NAME BLEEP-plumtree-broadcast
#   COMMAND python test-plumtree-broadcast.py
#   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME BLEEP-rc1-test
  COMMAND python test-rc1.py
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
