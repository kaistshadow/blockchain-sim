## required for independent build (currently not supported)
if (NOT TARGET BLEEP) # check whether it's independent build
    message(FATAL_ERROR "Did you try independent build? Only the full-build (building entire project) is supported.")
endif ()
## required for independent build end

## enable CTEST
enable_testing()

if (NOT CMAKE_DEBUG_POSTFIX)
    set(CMAKE_DEBUG_POSTFIX d)
endif ()

######## prepare monero plugin for experiment
add_custom_target(verack-monero-copy-monero-plugin ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:shadow_monero_daemon> ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(verack-monero-copy-monero-plugin shadow_monero_daemon)


######## compile node which uses sybil-API
add_shadow_plugin(VERACK_MONERO_TESTER test_verack_with_monero.cpp)
target_link_libraries(VERACK_MONERO_TESTER BLEEPsybil)
target_link_libraries(VERACK_MONERO_TESTER MoneroSybilAdapter)

######## prepare(copy) node which uses sybil-API
add_custom_command(TARGET VERACK_MONERO_TESTER
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:VERACK_MONERO_TESTER> ${CMAKE_CURRENT_SOURCE_DIR})

######## run test : Run experiment and check result of the experiment
add_test(NAME test_verack_monero
        COMMAND python3 check.py $<TARGET_FILE:shadow> $<$<CONFIG:Debug>:topologyd.xml>$<$<CONFIG:Release>:topology.xml>
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

############### For 2 monero nodes ################
######## compile node which uses sybil-API
add_shadow_plugin(VERACK_MONERO_TESTER_TWO test_verack_with_twomonero.cpp)
target_link_libraries(VERACK_MONERO_TESTER_TWO BLEEPsybil)
target_link_libraries(VERACK_MONERO_TESTER_TWO MoneroSybilAdapter)

######## prepare(copy) node which uses sybil-API
add_custom_command(TARGET VERACK_MONERO_TESTER_TWO
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:VERACK_MONERO_TESTER_TWO> ${CMAKE_CURRENT_SOURCE_DIR})
######## run test
add_test(NAME test_verack_twomonero
        COMMAND python3 check_two.py $<TARGET_FILE:shadow> $<$<CONFIG:Debug>:topology_twod.xml>$<$<CONFIG:Release>:topology_two.xml>
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})