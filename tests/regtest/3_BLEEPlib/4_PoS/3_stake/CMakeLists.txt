add_custom_target(pos_regtest_3_stake_test ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BLEEPemul-test-pos-node> ${CMAKE_CURRENT_SOURCE_DIR}
)

add_dependencies(pos_regtest_3_stake_test BLEEPemul-test-pos-node)

add_test(NAME 3_POS_stake
        COMMAND python stake.py
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)