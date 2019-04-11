# register the test
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data/bcdnode1)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data/bcdnode2)
add_test(NAME bitcoin-version-handshake 
  COMMAND ${SHADOW_ROOT}/bin/shadow ${CMAKE_CURRENT_SOURCE_DIR}/example.xml
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})


