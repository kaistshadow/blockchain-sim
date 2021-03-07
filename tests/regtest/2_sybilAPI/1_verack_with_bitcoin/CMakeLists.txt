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

######## prepare bitcoin plugin for bitcoin experiment
add_custom_target(verack-bitcoin-prepare ALL
        COMMAND rm -rf data/bcdnode0
        COMMAND mkdir -p data/bcdnode0
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_custom_target(verack-bitcoin-copy-bitcoin-plugin ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Bitcoin_0_19_1dev::Plugin> ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(verack-bitcoin-copy-bitcoin-plugin Bitcoin_0_19_1dev::Plugin)


######## compile node which uses sybil-API
add_shadow_plugin(VERACK_BITCOIN_TESTER test_verack_with_bitcoin.cpp
        BitcoinNodePrimitives.cpp
        BitcoinNodePrimitives.h)
target_link_libraries(VERACK_BITCOIN_TESTER BLEEPsybil)
add_dependencies(VERACK_BITCOIN_TESTER BLEEPsybil)

######## prepare(copy) node which uses sybil-API
add_custom_command(TARGET VERACK_BITCOIN_TESTER
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:VERACK_BITCOIN_TESTER> ${CMAKE_CURRENT_SOURCE_DIR})

######## run test
### step 1. Run experiment and check result of the experiment
add_test(NAME test_verack_bitcoin
        COMMAND python3 check.py $<TARGET_FILE:shadow>
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})