## required for independent build (currently not supported)
if (NOT TARGET BLEEP) # check whether it's independent build
    message(FATAL "Did you try independent build? Independent build is not supported.")
endif ()
## required for independent build end


## enable CTEST
enable_testing()

####
add_shadow_plugin(ATTACK_TESTER attack-test.cpp)
add_dependencies(ATTACK_TESTER SIMPLE_NODE)
target_link_libraries(ATTACK_TESTER BLEEPsybil)
add_custom_command(TARGET ATTACK_TESTER
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:ATTACK_TESTER> ${CMAKE_CURRENT_SOURCE_DIR})