# Currently BLEEPsybil doesn't support installation.
# In order to use BLEEPsybil, it should be compiled along with test example
add_library(BLEEPsybil SHARED)
set_target_properties(BLEEPsybil PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(BLEEPsybil SHADOW_INTERFACE)
target_link_libraries(BLEEPsybil Libev_4_33::Libev)

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
        policy/EclipseIncomingAttackPolicy.h
        policy/ErebusAttackPolicy.h
        utility/AttackStat.h
        utility/TCPControl.h
        )

target_include_directories(BLEEPsybil
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
