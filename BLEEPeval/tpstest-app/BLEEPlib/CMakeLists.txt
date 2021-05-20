if (NOT CMAKE_INSTALL_PREFIX OR
        CMAKE_INSTALL_PREFIX STREQUAL "/usr/local")
  set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/../../../Install")
endif()

# compile BLEEPlib P2P tester
add_library(BLEEPlibTPSAdapter SHARED
        BitcoinNodePrimitives.cpp
        BitcoinNodePrimitives.h)
target_link_libraries(BLEEPlibTPSAdapter BLEEPtps)
target_link_libraries(BLEEPlibTPSAdapter BLEEP)
target_include_directories(BLEEPlibTPSAdapter
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR})

add_shadow_plugin(BLEEPlibTPS_TESTER)
target_link_libraries(BLEEPlibTPS_TESTER BLEEPtps)
target_link_libraries(BLEEPlibTPS_TESTER BLEEPlibTPSAdapter)

target_sources(BLEEPlibTPS_TESTER
        PRIVATE
        tps_tester.cpp)

add_custom_target(tps_test2 ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BLEEPlibTPS_TESTER> ${CMAKE_INSTALL_PREFIX}/plugins
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BLEEPlibTPS_TESTER> ${CMAKE_CURRENT_SOURCE_DIR}
        )

add_shadow_plugin(BLEEPlib-pow-node)

#add_shadow_plugin(BLEEPlib-pow-node2 node-pow.cpp)
target_link_libraries(BLEEPlib-pow-node
        BLEEP)
target_sources(BLEEPlib-pow-node
        PRIVATE
        node-pow.cpp)
add_custom_target(bleeplib-pow-node ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BLEEPlib-pow-node> ${CMAKE_INSTALL_PREFIX}/plugins
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BLEEPlib-pow-node> ${CMAKE_CURRENT_SOURCE_DIR}
        )
## copy resulting plugins into source directory
#add_custom_command(TARGET BLEEPlib-pow-node2
#        POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BLEEPlib-pow-node2> ${CMAKE_CURRENT_SOURCE_DIR})


# Define targets that can be used in same build-tree
add_library(BLEEPlib_TPSTESTER::Plugin SHARED IMPORTED GLOBAL)
set_target_properties(BLEEPlib_TPSTESTER::Plugin PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES "C"
        IMPORTED_LOCATION ${CMAKE_INSTALL_PREFIX}/plugins/libBITCOINTPS_TESTER.so
        INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/src)
set_property(TARGET BLEEPlib_TPSTESTER::Plugin PROPERTY IMPORTED_NO_SONAME 1)


#build latency_tester.cpp
add_shadow_plugin(BLEEPlibLATENCY_TESTER)
target_link_libraries(BLEEPlibLATENCY_TESTER BLEEPtps)
target_link_libraries(BLEEPlibLATENCY_TESTER BitcoinTPSAdapter)
target_link_libraries(BLEEPlibLATENCY_TESTER Bitcoin_0_19_1dev::Plugin)

target_sources(BLEEPlibLATENCY_TESTER
        PRIVATE
        latency_tester.cpp)

add_custom_target(BLEEPlib_latency_test ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BLEEPlibLATENCY_TESTER> ${CMAKE_INSTALL_PREFIX}/plugins
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BLEEPlibLATENCY_TESTER> ${CMAKE_CURRENT_SOURCE_DIR}
        )