add_custom_target(cert_2_emulation_processing ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BLEEPlib-pow-node> ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(cert_2_emulation_processing BLEEPlib-pow-node)
