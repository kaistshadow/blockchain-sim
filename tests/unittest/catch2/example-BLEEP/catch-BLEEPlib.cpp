//
// Created by Yonggon Kim on 01/02/2021.
//
#define CATCH_CONFIG_MAIN

#include "catch.hpp"

#include "ev++.h"
#include "BL_MainEventManager.h"
#include "BL1_socket/SocketLayer_API.h"
#include "BL2_peer_connectivity/PeerConnectivityLayer_API.h"
#include "BL3_protocol/ProtocolLayer_API.h"
#include "BL3_protocol/ProtocolLayerPoW.h"

#include <chrono>


TEST_CASE("BLEEP-API-initialize") {
    using namespace libBLEEP_BL;

    SECTION("Get instance after initialization") {
        MainEventManager::InitInstance();
        BL_SocketLayer_API::Instance();
        BL_PeerConnectivityLayer_API::InitInstance("testnode1");

        REQUIRE(MainEventManager::Instance() != nullptr);
        REQUIRE(BL_SocketLayer_API::Instance() != nullptr);
        REQUIRE(BL_PeerConnectivityLayer_API::Instance() != nullptr);
    }

}

TEST_CASE("BLEEP-Lib-PoW-initialize") {
    using namespace libBLEEP_BL;

    SECTION("Get instance after initialization") {
        MainEventManager::InitInstance();
        BL_SocketLayer_API::Instance();
        BL_PeerConnectivityLayer_API::InitInstance("testnode1");
        BL_ProtocolLayer_API::InitInstance("PoW");

        REQUIRE(MainEventManager::Instance() != nullptr);
        REQUIRE(BL_SocketLayer_API::Instance() != nullptr);
        REQUIRE(BL_PeerConnectivityLayer_API::Instance() != nullptr);
        REQUIRE(BL_ProtocolLayer_API::Instance() != nullptr);
        REQUIRE(BL_ProtocolLayer_API::Instance()->InitiateProtocol() == true);
    }

    SECTION("Initiate again after initialization") {
        REQUIRE(BL_ProtocolLayer_API::Instance() != nullptr);
        REQUIRE(BL_ProtocolLayer_API::Instance()->InitiateProtocol() == false);
    }

    SECTION("Checking TX generation") {
        using clock = std::chrono::system_clock;
        const auto before = clock::now();
        std::chrono::duration<double> time_elapsed = clock::now() - before;
        while (time_elapsed.count() < 4) {
            ev_run(EV_DEFAULT, EVRUN_ONCE);
            time_elapsed = clock::now() - before;
        }

        int txGeneratedNum = BL_ProtocolLayer_API::Instance()->GetTxGeneratedNum();
        REQUIRE(txGeneratedNum > 0);
        REQUIRE(BL_ProtocolLayer_API::Instance()->GetTxPool() != nullptr);
        REQUIRE(txGeneratedNum == BL_ProtocolLayer_API::Instance()->GetTxPool()->GetPendingTxNum());

        REQUIRE(BL_ProtocolLayer_API::Instance()->GetTxGeneratedNum() >= 2);
        REQUIRE(BL_ProtocolLayer_API::Instance()->GetTxGeneratedNum() ==
                BL_ProtocolLayer_API::Instance()->GetTxPool()->GetPendingTxNum());
    }
}

// parameters for PoW
//int libBLEEP_BL::txNumPerBlock = 2;
//double libBLEEP_BL::txGenStartAt = 0;
//double libBLEEP_BL::txGenInterval = 4;
//double libBLEEP_BL::miningtime = 2;
//int libBLEEP_BL::miningnodecnt = 1;

TEST_CASE("BLEEP-Lib-PoW-mining") {
    using namespace libBLEEP_BL;
    using clock = std::chrono::system_clock;
    const auto before = clock::now();

    SECTION("Get instance after initialization") {
        MainEventManager::InitInstance(10,AsyncEventEnum::AllEvent);
        BL_SocketLayer_API::Instance();
        BL_PeerConnectivityLayer_API::InitInstance("testnode1");
        BL_ProtocolLayer_API::InitInstance("PoW");

        REQUIRE(MainEventManager::Instance() != nullptr);
        REQUIRE(BL_SocketLayer_API::Instance() != nullptr);
        REQUIRE(BL_PeerConnectivityLayer_API::Instance() != nullptr);
        REQUIRE(BL_ProtocolLayer_API::Instance() != nullptr);
        REQUIRE(BL_ProtocolLayer_API::Instance()->InitiateProtocol() == true);
    }

    SECTION("Checking Block generation") {
        MainEventManager::Instance()->Wait();
        std::chrono::duration<double> time_elapsed = clock::now() - before;

        REQUIRE(time_elapsed.count() >= 9.9);

        int txGeneratedNum = BL_ProtocolLayer_API::Instance()->GetTxGeneratedNum();
        REQUIRE(txGeneratedNum > 0);
        REQUIRE(BL_ProtocolLayer_API::Instance()->GetTxPool() != nullptr);
        REQUIRE(txGeneratedNum == BL_ProtocolLayer_API::Instance()->GetTxPool()->GetPendingTxNum() +
                                  BL_ProtocolLayer_API::Instance()->GetBlockPoolSize() * 2);
        REQUIRE(BL_ProtocolLayer_API::Instance()->GetBlockPoolSize() == 1);
    }

    SECTION("Checking BlockTree") {
        BL_ProtocolLayerPoW* powAPI = dynamic_cast<BL_ProtocolLayerPoW*>(BL_ProtocolLayer_API::Instance());
        REQUIRE(powAPI != nullptr);
        BlockTree<POWBlock>& blockTree = powAPI->GetBlockTree();

        REQUIRE(blockTree.GetNextBlockIdx() == BL_ProtocolLayer_API::Instance()->GetBlockPoolSize());
        REQUIRE(blockTree.GetLastHash() == blockTree.GetBlock(blockTree.GetLastHash().str())->GetBlockHash());
    }
}


//int libBLEEP_BL::txNumPerBlock = 2;
//double libBLEEP_BL::txGenStartAt = 0;
//double libBLEEP_BL::txGenInterval = 4;
//double libBLEEP_BL::miningtime = 2;
//int libBLEEP_BL::miningnodecnt = 1;
TEST_CASE("BLEEP-Lib-PoW-mining-long") {
    using namespace libBLEEP_BL;
    using clock = std::chrono::system_clock;
    const auto before = clock::now();

    SECTION("Get instance after initialization") {
        MainEventManager::InitInstance(32,AsyncEventEnum::AllEvent);
        BL_SocketLayer_API::Instance();
        BL_PeerConnectivityLayer_API::InitInstance("testnode1");
        BL_ProtocolLayer_API::InitInstance("PoW");

        REQUIRE(MainEventManager::Instance() != nullptr);
        REQUIRE(BL_SocketLayer_API::Instance() != nullptr);
        REQUIRE(BL_PeerConnectivityLayer_API::Instance() != nullptr);
        REQUIRE(BL_ProtocolLayer_API::Instance() != nullptr);
        REQUIRE(BL_ProtocolLayer_API::Instance()->InitiateProtocol() == true);
    }

    SECTION("Checking BlockPool") {
        MainEventManager::Instance()->Wait();
        std::chrono::duration<double> time_elapsed = clock::now() - before;

        REQUIRE(time_elapsed.count() >= 30);

        int txGeneratedNum = BL_ProtocolLayer_API::Instance()->GetTxGeneratedNum();
        REQUIRE(txGeneratedNum > 0);
        REQUIRE(txGeneratedNum < 10);
        REQUIRE(BL_ProtocolLayer_API::Instance()->GetTxPool() != nullptr);
        REQUIRE(txGeneratedNum == BL_ProtocolLayer_API::Instance()->GetTxPool()->GetPendingTxNum() +
                                  BL_ProtocolLayer_API::Instance()->GetBlockPoolSize() * 2);
        REQUIRE(BL_ProtocolLayer_API::Instance()->GetBlockPoolSize() == 4);
    }

    SECTION("Checking BlockTree") {
        BL_ProtocolLayerPoW* powAPI = dynamic_cast<BL_ProtocolLayerPoW*>(BL_ProtocolLayer_API::Instance());
        REQUIRE(powAPI != nullptr);
        BlockTree<POWBlock>& blockTree = powAPI->GetBlockTree();

        REQUIRE(blockTree.GetNextBlockIdx() == 4);
        REQUIRE(blockTree.GetLastHash() == blockTree.GetBlock(blockTree.GetLastHash().str())->GetBlockHash());
    }
}