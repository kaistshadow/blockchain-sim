add_custom_target(cert_2_consensus_pos ALL
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BLEEPemul-test-pos-node> ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(cert_2_consensus_pos BLEEPemul-test-pos-node)