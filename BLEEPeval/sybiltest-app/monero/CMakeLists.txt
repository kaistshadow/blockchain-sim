# compile Monero adapter
add_library(MoneroSybilAdapter SHARED
        MoneroNodeParams.h
        MoneroNodePrimitives.cpp
        MoneroNodePrimitives.h)
target_link_libraries(MoneroSybilAdapter BLEEPsybil)
target_link_libraries(MoneroSybilAdapter shadow_monero_daemon)
target_include_directories(MoneroSybilAdapter
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR})


add_shadow_plugin(MONEROP2P_TESTER)
target_link_libraries(MONEROP2P_TESTER BLEEPsybil)
target_link_libraries(MONEROP2P_TESTER MoneroSybilAdapter)

target_sources(MONEROP2P_TESTER
        PRIVATE
        tester.cpp)


# copy plugins into source directory
add_custom_target(sybil-API-monero-copy-plugin ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:shadow_monero_daemon> ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(sybil-API-monero-copy-plugin shadow_monero_daemon)

add_custom_command(TARGET MONEROP2P_TESTER
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:MONEROP2P_TESTER> ${CMAKE_CURRENT_SOURCE_DIR})
