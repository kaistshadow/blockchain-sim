add_custom_target(cert_2_consensus_pbft ALL
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BLEEPemul-test-pbft-node> ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(cert_2_consensus_pbft BLEEPemul-test-pbft-node)