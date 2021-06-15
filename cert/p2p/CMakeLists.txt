add_custom_target(cert_p2pTest2 ALL
    ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BLEEPlib-test-pow-node> ${CMAKE_CURRENT_SOURCE_DIR}
)
add_dependencies(cert_p2pTest2 BLEEPlib-test-pow-node)