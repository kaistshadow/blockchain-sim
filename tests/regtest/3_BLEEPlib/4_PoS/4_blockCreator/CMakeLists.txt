add_custom_target(pos_regtest_4_creator ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BLEEPemul-test-pos-node> ${CMAKE_CURRENT_SOURCE_DIR}
)

add_dependencies(pos_regtest_4_creator BLEEPemul-test-pos-node)

add_test(NAME 4_POS_blockCreator
        COMMAND python creator.py
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)