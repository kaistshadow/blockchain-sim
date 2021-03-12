

add_custom_target(copy-monero-plugin ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:shadow_monero_daemon> ${CMAKE_CURRENT_SOURCE_DIR})