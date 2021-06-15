add_custom_target(cert_p2pTest ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BLEEPlib-pow-node> ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(cert_p2pTest BLEEPlib-pow-node)
