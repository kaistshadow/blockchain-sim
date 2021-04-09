## required for independent build (currently not supported)
if (NOT TARGET BLEEP) # check whether it's independent build
    message(FATAL "Did you try independent build? Only the full-build (building entire project) is supported.")
endif ()
## required for independent build end

## enable CTEST
enable_testing()

if (NOT CMAKE_DEBUG_POSTFIX)
    set(CMAKE_DEBUG_POSTFIX d)
endif ()

######## prepare monero plugin for experiment
add_custom_target(shadownode-monero-copy-monero-plugin ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:shadow_monero_daemon> ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(shadownode-monero-copy-monero-plugin shadow_monero_daemon)


######## compile node which uses sybil-API
add_shadow_plugin(SHADOWNODE_MONERO_TESTER test_shadownode_with_monero.cpp)
target_link_libraries(SHADOWNODE_MONERO_TESTER BLEEPsybil)
target_link_libraries(SHADOWNODE_MONERO_TESTER MoneroSybilAdapter)

######## prepare(copy) node which uses sybil-API
add_custom_command(TARGET SHADOWNODE_MONERO_TESTER
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:SHADOWNODE_MONERO_TESTER> ${CMAKE_CURRENT_SOURCE_DIR})

######## run test : Run experiment and check result of the experiment
add_test(NAME test_shadownode_monero
        COMMAND python3 check.py $<TARGET_FILE:shadow> $<$<CONFIG:Debug>:topologyd.xml>$<$<CONFIG:Release>:topology.xml>
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})