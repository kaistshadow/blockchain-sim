add_custom_target(TPS_TEST ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BLEEPlibTPS_TESTER> ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(TPS_TEST BLEEPlibTPS_TESTER)

add_custom_target(TPS_POW_NODE ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BLEEPlib-pow-node> ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(TPS_POW_NODE BLEEPlib-pow-node)
