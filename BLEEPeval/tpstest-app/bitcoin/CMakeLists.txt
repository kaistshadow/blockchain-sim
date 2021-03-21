#add_shadow_plugin(BITCOINTPS_TESTER1)
add_executable(BITCOINTPS_TESTER1)
target_link_libraries(BITCOINTPS_TESTER1 BLEEPtps)
target_link_libraries(BITCOINTPS_TESTER1 Bitcoin_0_19_1dev::Plugin)

target_sources(BITCOINTPS_TESTER1
        PRIVATE
        BitcoinKey.cpp
        BitcoinState.cpp
        BitcoinTxGen.cpp
        tester.cpp)

add_custom_command(TARGET BITCOINTPS_TESTER1
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BITCOINTPS_TESTER1> ${CMAKE_CURRENT_SOURCE_DIR})






