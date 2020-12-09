#include "BL_MainEventManager.h"
#include "BL1_socket/SocketLayer.h"
#include "BL1_socket/SocketLayer_API.h"
#include "BL2_peer_connectivity/PeerConnectivityLayer.h"
#include "BL2_peer_connectivity/PeerConnectivityLayer_API.h"
#include "BL2_peer_connectivity/Peer.h"

#include "utility/ArgsManager.h"

using namespace libBLEEP;

std::unique_ptr<libBLEEP_BL::BL_SocketLayer_API> libBLEEP_BL::g_SocketLayer_API;
std::unique_ptr<libBLEEP_BL::BL_PeerConnectivityLayer_API> libBLEEP_BL::g_PeerConnectivityLayer_API;
std::unique_ptr<libBLEEP_BL::MainEventManager> libBLEEP_BL::g_mainEventManager;

int main(int argc, char *argv[]) {

    gArgs.ParseParameters(argc, argv);

    if (gArgs.IsArgSet("-?") || gArgs.IsArgSet("-h") || gArgs.IsArgSet("-help") || gArgs.IsArgSet("-version")) {
        std::string strUsage = gArgs.HelpMessage();
        std::cout << strUsage << "\n";
        return 0;
    }

    std::cout << "main started" << "\n";

    /* allocate mainEventManager */
    libBLEEP_BL::g_mainEventManager = std::unique_ptr<libBLEEP_BL::MainEventManager>(
            new libBLEEP_BL::MainEventManager(libBLEEP_BL::AsyncEventEnum::AllEvent));
    /* allocate socketlayer */
    libBLEEP_BL::g_SocketLayer_API = std::unique_ptr<libBLEEP_BL::BL_SocketLayer_API>(
            new libBLEEP_BL::BL_SocketLayer());
    /* allocate peerConnectivityLayer */
    libBLEEP_BL::g_PeerConnectivityLayer_API = std::unique_ptr<libBLEEP_BL::BL_PeerConnectivityLayer_API>(
            new libBLEEP_BL::BL_PeerConnectivityLayer(gArgs.GetArg("-id", "noid")));


    /* allocate High-level Protocol implementation */
    // Need to be filled
    // Need to be filled


    // Start a initial connection
    for (auto neighborPeerIdStr : gArgs.GetArgs("-connect"))
        libBLEEP_BL::g_PeerConnectivityLayer_API->ConnectPeer(libBLEEP_BL::PeerId(neighborPeerIdStr));

    libBLEEP_BL::g_mainEventManager->Wait(); // start main event loop (wait for next event)
}
