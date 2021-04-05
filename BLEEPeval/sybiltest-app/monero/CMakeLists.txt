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

# copy plugins into source directory
add_custom_target(sybil-API-monero-copy-plugin ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:shadow_monero_daemon> ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(sybil-API-monero-copy-plugin shadow_monero_daemon)

