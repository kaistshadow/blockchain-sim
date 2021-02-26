# Currently BLEEPsybil doesn't support installation.
# In order to use BLEEPsybil, it should be compiled along with test example
add_library(BLEEPsybil SHARED)

target_link_libraries(BLEEPsybil SHADOW_INTERFACE)
target_link_libraries(BLEEPsybil Libev_4_33::Libev_static)
target_link_libraries(BLEEPsybil BLEEP)

target_link_libraries(BLEEPsybil Bitcoin_0_19_1dev::Plugin)

target_sources(BLEEPsybil
        PRIVATE
        AttackBox.h
        ipdb/SimpleIPDatabase.h
        ipdb/BitcoinIPDatabase.h
        ipdb/IPDatabase.h
        node/BenignNode.h
        node/ShadowActiveNode.h
        node/ShadowNode.h
        node/Node.h
        nodePrimitives/BLNodePrimitives.h
        nodePrimitives/BLNodePrimitives.cpp
        nodePrimitives/BitcoinNodePrimitives.cpp
        nodePrimitives/BitcoinNodePrimitives.h
        nodeParams/BitcoinNodeParams.h
        nodeParams/BLNodeParams.h
        policy/EclipseIncomingAttackPolicy.h
        policy/ErebusAttackPolicy.h
        utility/AttackStat.h
        utility/TCPControl.h
        )

target_include_directories(BLEEPsybil
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
