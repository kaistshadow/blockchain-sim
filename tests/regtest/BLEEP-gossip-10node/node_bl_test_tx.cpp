#include "BL_MainEventManager.h"
#include "BL1_socket/SocketLayer_API.h"
#include "BL2_peer_connectivity/PeerConnectivityLayer_API.h"
#include "BL3_protocol/ProtocolLayer_API.h"
#include "BL_ShadowLayer.h"

#include "BL2_peer_connectivity/Peer.h"

#include "utility/ArgsManager.h"
#include "utility/GlobalClock.h"
#include "utility/Logger.h"

#include "crypto/SHA256.h"

#include <algorithm>
#include <random>


using namespace libBLEEP;


std::unique_ptr<libBLEEP_BL::ShadowLayer> libBLEEP_BL::g_ShadowLayer;

int main(int argc, char *argv[]) {

    gArgs.ParseParameters(argc, argv);

    if (gArgs.IsArgSet("-?") || gArgs.IsArgSet("-h") ||  gArgs.IsArgSet("-help") || gArgs.IsArgSet("-version")) {
        std::string strUsage = gArgs.HelpMessage();
        gLog << strUsage << "\n";
        return 0;
    }

    std::cout << "main started" << "\n";

    /* allocate mainEventManager */
    libBLEEP_BL::MainEventManager::InitInstance(libBLEEP_BL::AsyncEventEnum::AllEvent);
    std::string myId = gArgs.GetArg("-id", "noid");
    libBLEEP_BL::BL_PeerConnectivityLayer_API::InitInstance(myId);

    libBLEEP_BL::BL_ProtocolLayer_API::InitInstance("Ex1");

    /* allocate protocol */
    libBLEEP_BL::BL_ProtocolLayer_API::Instance()->InitiateProtocol();

    libBLEEP_BL::BL_SocketLayer_API::Instance();

    /* allocate Consensus Protocol implementation */
    // Need to be filled
    // Need to be filled
    // Need to be filled
    // Need to be filled


    /* allocate shadowLayer for debugging interface */
    libBLEEP_BL::g_ShadowLayer = std::unique_ptr<libBLEEP_BL::ShadowLayer>(new libBLEEP_BL::ShadowLayer());


    std::cout << "before connect" << "\n";
    for (auto neighborPeerIdStr : gArgs.GetArgs("-connect"))
        // libBLEEP_BL::g_SocketLayer_API->ConnectSocket(neighborPeerIdStr);
        libBLEEP_BL::BL_PeerConnectivityLayer_API::Instance()->ConnectPeer(libBLEEP_BL::PeerId(neighborPeerIdStr));

    std::cout << "after connect" << "\n";

    while(true) {
        std::cout << "while" << "\n";
        libBLEEP_BL::MainEventManager::Instance()->Wait(); // main event loop (wait for next event)

        // loop returned
        PrintTimespec("mainEventManager.Wait returned");

        libBLEEP_BL::AsyncEvent event = libBLEEP_BL::MainEventManager::Instance()->PopAsyncEvent();

        switch (event.GetType()) {
            case libBLEEP_BL::AsyncEventEnum::Layer1_Event_Start ... libBLEEP_BL::AsyncEventEnum::Layer1_Event_End:
                libBLEEP_BL::BL_SocketLayer_API::Instance()->SwitchAsyncEventHandler(event);
                // if (event.GetType() == libBLEEP_BL::AsyncEventEnum::SocketConnect) {
            //     libBLEEP_BL::g_SocketLayer_API->SendToSocket(event.GetData().GetNewlyConnectedSocket(), "hello", 5);
            //     libBLEEP_BL::g_SocketLayer_API->SendToSocket(event.GetData().GetNewlyConnectedSocket(), "world", 5);
            // }
            break;
        case libBLEEP_BL::AsyncEventEnum::Layer2_Event_Start ... libBLEEP_BL::AsyncEventEnum::Layer2_Event_End:
            libBLEEP_BL::BL_PeerConnectivityLayer_API::Instance()->SwitchAsyncEventHandler(event);
            break;
        }
    }
}
