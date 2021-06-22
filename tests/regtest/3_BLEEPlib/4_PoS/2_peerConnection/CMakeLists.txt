add_custom_target(pos_regtest_2_peerConnection ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BLEEPemul-test-pos-node> ${CMAKE_CURRENT_SOURCE_DIR}
)

add_dependencies(pos_regtest_2_peerConnection BLEEPemul-test-pos-node)

add_test(NAME 2_POS_peerConnection
        COMMAND python peerConnection.py
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)