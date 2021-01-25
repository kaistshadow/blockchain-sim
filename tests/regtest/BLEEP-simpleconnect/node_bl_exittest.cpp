#include "BL_MainEventManager.h"
#include "BL1_socket/SocketLayer.h"
#include "BL1_socket/SocketLayer_API.h"
#include "BL2_peer_connectivity/PeerConnectivityLayer.h"
#include "BL2_peer_connectivity/PeerConnectivityLayer_API.h"
#include "BL_ShadowLayer.h"


#include "BL2_peer_connectivity/Peer.h"

#include "utility/ArgsManager.h"
#include "utility/GlobalClock.h"
#include "utility/Logger.h"

#include "crypto/SHA256.h"

#include <algorithm>
#include <random>


using namespace libBLEEP;


std::unique_ptr<libBLEEP_BL::BL_SocketLayer_API> libBLEEP_BL::g_SocketLayer_API;
std::unique_ptr<libBLEEP_BL::BL_PeerConnectivityLayer_API> libBLEEP_BL::g_PeerConnectivityLayer_API;
std::unique_ptr<libBLEEP_BL::MainEventManager> libBLEEP_BL::g_mainEventManager;
std::unique_ptr<libBLEEP_BL::ShadowLayer> libBLEEP_BL::g_ShadowLayer;

class ExitTimer {
private:
    ev::timer _exit_timer;

    void _exit_timercallback(ev::timer &w, int revents) {
        std::cout << "exitTimer activated!" << "\n";
        exit(0);
    }

public:
    ExitTimer(double time) {
        _exit_timer.set<ExitTimer, &ExitTimer::_exit_timercallback>(this);
        _exit_timer.set(time, 0.);
        _exit_timer.start();
        std::cout << "closeListenSocketTimer started" << "\n";
    }
};

int main(int argc, char *argv[]) {

    gArgs.ParseParameters(argc, argv);

    if (gArgs.IsArgSet("-?") || gArgs.IsArgSet("-h") || gArgs.IsArgSet("-help") || gArgs.IsArgSet("-version")) {
        std::string strUsage = gArgs.HelpMessage();
        gLog << strUsage << "\n";
        return 0;
    }

    std::cout << "main started" << "\n";

    /* allocate mainEventManager */
    libBLEEP_BL::g_mainEventManager = std::unique_ptr<libBLEEP_BL::MainEventManager>(
            new libBLEEP_BL::MainEventManager());
    /* allocate socketlayer */
    libBLEEP_BL::g_SocketLayer_API = std::unique_ptr<libBLEEP_BL::BL_SocketLayer_API>(
            new libBLEEP_BL::BL_SocketLayer());
    /* allocate peerConnectivityLayer */
    std::string myId = gArgs.GetArg("-id", "noid");
    libBLEEP_BL::g_PeerConnectivityLayer_API = std::unique_ptr<libBLEEP_BL::BL_PeerConnectivityLayer_API>(
            new libBLEEP_BL::BL_PeerConnectivityLayer(myId));
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
        libBLEEP_BL::g_PeerConnectivityLayer_API->ConnectPeer(libBLEEP_BL::PeerId(neighborPeerIdStr));

    std::cout << "after connect" << "\n";

    // for testing purpose, initiate close timer
    ExitTimer timer(30);


    while (true) {
        std::cout << "while" << "\n";
        libBLEEP_BL::g_mainEventManager->Wait(); // main event loop (wait for next event)

        // loop returned
        PrintTimespec("mainEventManager.Wait returned");

        libBLEEP_BL::AsyncEvent event = libBLEEP_BL::g_mainEventManager->PopAsyncEvent();

        switch (event.GetType()) {
            case libBLEEP_BL::AsyncEventEnum::Layer1_Event_Start ... libBLEEP_BL::AsyncEventEnum::Layer1_Event_End:
                if (event.GetType() == libBLEEP_BL::AsyncEventEnum::SocketWrite) {
                    std::cout << "writeSocket fd=[" << event.GetData().GetWriteSocket() << "]" << "\n";
                }
                libBLEEP_BL::g_SocketLayer_API->SwitchAsyncEventHandler(event);
                // if (event.GetType() == libBLEEP_BL::AsyncEventEnum::SocketConnect) {
                //     libBLEEP_BL::g_SocketLayer_API->SendToSocket(event.GetData().GetNewlyConnectedSocket(), "hello", 5);
                //     libBLEEP_BL::g_SocketLayer_API->SendToSocket(event.GetData().GetNewlyConnectedSocket(), "world", 5);
                // }
                break;
            case libBLEEP_BL::AsyncEventEnum::Layer2_Event_Start ... libBLEEP_BL::AsyncEventEnum::Layer2_Event_End:
                libBLEEP_BL::g_PeerConnectivityLayer_API->SwitchAsyncEventHandler(event);
                break;
        }
    }
}
