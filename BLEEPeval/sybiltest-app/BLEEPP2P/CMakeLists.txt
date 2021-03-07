
add_shadow_plugin(BLEEPP2P_TESTER)
target_link_libraries(BLEEPP2P_TESTER BLEEPsybil)
target_link_libraries(BLEEPP2P_TESTER BLEEP)
target_sources(BLEEPP2P_TESTER
        PRIVATE
        tester.cpp
        BLNodeParams.h
        BLNodePrimitives.cpp
        BLNodePrimitives.h)
target_include_directories(BLEEPP2P_TESTER
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR})

add_shadow_plugin(BLEEPP2P_TARGET target_node.cpp)
target_link_libraries(BLEEPP2P_TARGET BLEEP)

# copy resulting plugins into source directory
add_custom_command(TARGET BLEEPP2P_TARGET
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BLEEPP2P_TARGET> ${CMAKE_CURRENT_SOURCE_DIR})
add_custom_command(TARGET BLEEPP2P_TESTER
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BLEEPP2P_TESTER> ${CMAKE_CURRENT_SOURCE_DIR})
