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
        REQUIRE(BL_ProtocolLayer_API::Instance()->GetTxGeneratedNum() == BL_ProtocolLayer_API::Instance()->GetTxPool()->GetPendingTxNum());
    }
}