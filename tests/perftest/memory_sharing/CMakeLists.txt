add_custom_target(memory_dedup ALL
    ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BLEEPlib-test-pow-node> ${CMAKE_CURRENT_SOURCE_DIR}
)
add_dependencies(memory_dedup BLEEPlib-test-pow-node)