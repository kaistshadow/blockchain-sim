# Currently BLEEPsybil doesn't support installation.
# In order to use BLEEPsybil, it should be compiled along with test example
add_library(BLEEPsybil SHARED)

target_link_libraries(BLEEPsybil SHADOW_INTERFACE)
target_link_libraries(BLEEPsybil Libev_4_33::Libev)
target_link_libraries(BLEEPsybil BLEEP)

target_sources(BLEEPsybil
        PRIVATE
        AttackBox.h
        ipdb/IPDatabase.cpp
        ipdb/IPDatabase.h
        ipdb/BitcoinIPDatabase.cpp
        ipdb/BitcoinIPDatabase.h
        node/BenignNode.h
        node/ShadowActiveNode.h
        node/Node.h
        nodePrimitives/BLNodePrimitives.h
        nodePrimitives/BLNodePrimitives.cpp
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
