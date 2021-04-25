//
// Created by Yonggon Kim on 01/02/2021.
//
#define CATCH_CONFIG_MAIN

#include "catch.hpp"

#include "BL_MainEventManager.h"
#include "BL1_socket/SocketLayer_API.h"
#include "BL2_peer_connectivity/PeerConnectivityLayer_API.h"
#include "BL3_protocol/ProtocolLayer_API.h"


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

        REQUIRE(MainEventManager::Instance() != nullptr);
        REQUIRE(BL_SocketLayer_API::Instance() != nullptr);
        REQUIRE(BL_PeerConnectivityLayer_API::Instance() != nullptr);

        BL_ProtocolLayer_API::InitInstance("PoW");
        REQUIRE(BL_ProtocolLayer_API::Instance() != nullptr);
        REQUIRE(BL_ProtocolLayer_API::Instance()->InitiateProtocol() == true);
    }
}