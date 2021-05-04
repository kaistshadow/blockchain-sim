add_library(BLEEPtps SHARED)
set_target_properties(BLEEPtps PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(BLEEPtps SHADOW_INTERFACE)
target_link_libraries(BLEEPtps Libev_4_33::Libev)
target_link_libraries(BLEEPtps Bitcoin_0_19_1dev::Plugin)

target_sources(BLEEPtps
        PRIVATE
        TxBox.h
        node/TxGeneratorNode.h
        node/Node.h
        node/MonitoringNode.h
        policy/TPSPolicy.h
        policy/LatencyPolicy.h
        utility/blocks.h
        utility/blocks.cpp
        )

target_include_directories(BLEEPtps
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

