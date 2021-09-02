# copy bitcoind plugin
add_custom_target(4_Difficulty_BLEEP_LIB_REGTESTER ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BLEEPlib-pow-node> ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(4_Difficulty_BLEEP_LIB_REGTESTER BLEEPlib-pow-node)

add_test(NAME BLEEP_LIB_4_DIFFICULTY_TEST
  COMMAND python3 difficulty.py
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
set_property(TEST BLEEP_LIB_4_DIFFICULTY_TEST APPEND
        PROPERTY
        ENVIRONMENT PATH=$ENV{PATH}:$<TARGET_FILE_DIR:shadow>) # add path environment in order to run this test on CLion