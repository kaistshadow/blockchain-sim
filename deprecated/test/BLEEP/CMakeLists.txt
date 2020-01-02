# register the test

add_test(NAME BLEEP-nonblock-io
  COMMAND python test-nonblock-io.py
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME BLEEP-asio
  COMMAND python test-asio.py
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME BLEEP-libev
  COMMAND python test-libev.py
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# test for old BLEEP
# add_test(NAME BLEEP-rc1-test
#   COMMAND python test-rc1.py
#   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME BLEEP-libBLEEP-test
  COMMAND python test-rc1-fsm.py
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
