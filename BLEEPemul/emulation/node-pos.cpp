#include "BL_MainEventManager.h"
#include "BL1_socket/SocketLayer_API.h"
#include "BL2_peer_connectivity/PeerConnectivityLayer_API.h"
#include "BL3_protocol/ProtocolLayer_API.h"
#include "BL3_protocol/ProtocolLayerPoS.h"
#include "BL3_protocol/POSProtocolParameter.h"

#include "utility/ArgsManager.h"

using namespace libBLEEP_BL;
using namespace libBLEEP;

int main(int argc, char *argv[]) {

    gArgs.ParseParameters(argc, argv);

    if (gArgs.IsArgSet("-?") || gArgs.IsArgSet("-h") ||  gArgs.IsArgSet("-help") || gArgs.IsArgSet("-version")) {
        std::string strUsage = gArgs.HelpMessage();
        std::cout << strUsage << "\n";
        return 0;
    }

    std::cout << "main started" << "\n";
    std::string myId = gArgs.GetArg("-id", "noid");

    POSProtocolParameter posparams;
    if (gArgs.IsArgSet("-txgenstartat"))
        posparams.txGenStartAt = std::stof(gArgs.GetArg("-txgenstartat"));
    if (gArgs.IsArgSet("-txgeninterval"))
        posparams.txGenInterval = std::stof(gArgs.GetArg("-txgeninterval"));
    if (gArgs.IsArgSet("-creatorid"))
        posparams.creatorNodeId = std::stoul(gArgs.GetArg("-creatorid"));
    if (gArgs.IsArgSet("-stakefile"))
        posparams.stakeDatafile = gArgs.GetArg("-stakefile");
    if (gArgs.IsArgSet("-slotinterval")) {
        posparams.slot_interval = std::stod(gArgs.GetArg("-slotinterval"));
    }

    /* init BLEEP library components */
    MainEventManager::InitInstance(AsyncEventEnum::AllEvent);
    BL_SocketLayer_API::Instance();
    BL_PeerConnectivityLayer_API::InitInstance(myId);
    BL_ProtocolLayer_API::InitInstance("PoS");
    BL_ProtocolLayer_API::Instance()->InitiateProtocol(&posparams);

    std::cout << "before connect" << "\n";
    for (auto neighborPeerIdStr : gArgs.GetArgs("-connect"))
        // libBLEEP_BL::g_SocketLayer_API->ConnectSocket(neighborPeerIdStr);
        BL_PeerConnectivityLayer_API::Instance()->ConnectPeer(PeerId(neighborPeerIdStr));
    std::cout << "after connect" << "\n";

    /* loop mainEventManager */
    while(true) {
        std::cout << "while" << "\n";
        MainEventManager::Instance()->Wait(); // main event loop (wait for next event)

        // loop returned
        PrintTimespec("mainEventManager.Wait returned");

        libBLEEP_BL::AsyncEvent event = libBLEEP_BL::MainEventManager::Instance()->PopAsyncEvent();
        std::cout << (int)event.GetType() << "\n";
    }
}