## required for independent build (currently not supported)
if (NOT TARGET BLEEP) # check whether it's independent build
    message(FATAL "Did you try independent build? Independent build is not supported.")
endif ()
## required for independent build end


## enable CTEST
enable_testing()

if (NOT CMAKE_DEBUG_POSTFIX)
    set(CMAKE_DEBUG_POSTFIX d)
endif ()

####
add_shadow_plugin(ATTACK_TESTER attack-test.cpp)
add_dependencies(ATTACK_TESTER SIMPLE_NODE)
target_link_libraries(ATTACK_TESTER BLEEPsybil)
add_custom_command(TARGET ATTACK_TESTER
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:ATTACK_TESTER> ${CMAKE_CURRENT_SOURCE_DIR})


######### bitcoin experiment
add_custom_target(sybil-API-bitcoin-prepare ALL
        COMMAND rm -rf data/bcdnode0
        COMMAND mkdir -p data/bcdnode0
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(sybil-API-bitcoin-copy-plugin ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Bitcoin_0_19_1dev::Plugin> ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(sybil-API-bitcoin-copy-plugin Bitcoin_0_19_1dev::Plugin)


add_shadow_plugin(ATTACK_BITCOIN_EREBUS_TESTER attack-bitcoin-erebus-test.cpp)
target_link_libraries(ATTACK_BITCOIN_EREBUS_TESTER BLEEPsybil)
add_dependencies(ATTACK_BITCOIN_EREBUS_TESTER BLEEPsybil)
add_custom_command(TARGET ATTACK_BITCOIN_EREBUS_TESTER
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:ATTACK_BITCOIN_EREBUS_TESTER> ${CMAKE_CURRENT_SOURCE_DIR})


#####temporary
#add_shadow_plugin(ATTACK_TESTER2 attack-bitcoin-erebus-test2.cpp)
#add_dependencies(ATTACK_TESTER2 SIMPLE_NODE)
#target_link_libraries(ATTACK_TESTER2 BLEEPsybil)
#add_custom_command(TARGET ATTACK_TESTER2
#        POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:ATTACK_TESTER2> ${CMAKE_CURRENT_SOURCE_DIR})
