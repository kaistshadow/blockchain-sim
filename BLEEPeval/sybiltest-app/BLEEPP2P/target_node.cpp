//
// Created by Yonggon Kim on 07/03/2021.
//

// 1. Refer to the header files that declare the layered API layers.
#include "BL_MainEventManager.h"
#include "BL1_socket/SocketLayer_API.h"
#include "BL2_peer_connectivity/PeerConnectivityLayer_API.h"

// 2. Define a variable pointing to the tiered API layer interface
using namespace libBLEEP;
using namespace libBLEEP_BL;


// 3. Header file reference for utility functions
#include "utility/ArgsManager.h"


int main(int argc, char *argv[]) {
    gArgs.ParseParameters(argc, argv);

    MainEventManager::InitInstance(AsyncEventEnum::AllEvent);
    BL_SocketLayer_API::Instance();
    BL_PeerConnectivityLayer_API::InitInstance(gArgs.GetArg("-id", "noid"));

    for (auto neighborPeerIdStr : gArgs.GetArgs("-connect"))
        BL_PeerConnectivityLayer_API::Instance()->ConnectPeer(PeerId(neighborPeerIdStr));

    MainEventManager::Instance()->Wait();

    return 0;
}






