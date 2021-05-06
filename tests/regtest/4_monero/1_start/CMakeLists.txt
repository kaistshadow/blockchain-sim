## required for independent build (currently not supported)
if (NOT TARGET shadow_monero_daemon) # check whether it's independent build
    message(FATAL_ERROR "Did you try independent build? Only the full-build (building entire project) is supported.")
endif ()
## required for independent build end

add_custom_target(start-monero-copy-monero-plugin ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:shadow_monero_daemon> ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(start-monero-copy-monero-plugin shadow_monero_daemon)


######## run test
### Run monero plugin and check standard output of the plugin
add_test(NAME test_start_monero
        COMMAND python3 check.py $<TARGET_FILE:shadow>
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})