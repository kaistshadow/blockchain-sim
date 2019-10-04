#include "BL_MainEventManager.h"
#include "BL1_socket/SocketLayer.h"
#include "BL1_socket/SocketLayer_API.h"


#include "utility/ArgsManager.h"
#include "utility/GlobalClock.h"
#include "utility/Logger.h"

#include "crypto/SHA256.h"

#include <algorithm>
#include <random>


using namespace libBLEEP;


std::unique_ptr<libBLEEP_BL::BL_SocketLayer_API> libBLEEP_BL::g_SocketLayer_API;
std::unique_ptr<libBLEEP_BL::MainEventManager> libBLEEP_BL::g_mainEventManager;

int main(int argc, char *argv[]) {

    gArgs.ParseParameters(argc, argv);

    if (gArgs.IsArgSet("-?") || gArgs.IsArgSet("-h") ||  gArgs.IsArgSet("-help") || gArgs.IsArgSet("-version")) {
        std::string strUsage = gArgs.HelpMessage();
        gLog << strUsage << "\n";
        return 0;
    }

    std::cout << "main started" << "\n";

    /* allocate mainEventManager */
    libBLEEP_BL::g_mainEventManager = std::unique_ptr<libBLEEP_BL::MainEventManager>(new libBLEEP_BL::MainEventManager());
    /* allocate socketlayer */
    libBLEEP_BL::g_SocketLayer_API = std::unique_ptr<libBLEEP_BL::BL_SocketLayer_API>(new libBLEEP_BL::BL_SocketLayer());

    std::string myId = gArgs.GetArg("-id", "noid");

    std::cout << "before connect" << "\n";
    for (auto neighborPeerIdStr : gArgs.GetArgs("-connect"))
        libBLEEP_BL::g_SocketLayer_API->ConnectSocket(neighborPeerIdStr);

    std::cout << "after connect" << "\n";

    while(true) {
        std::cout << "while" << "\n";
        libBLEEP_BL::g_mainEventManager->Wait(); // main event loop (wait for next event)

        // loop returned
        PrintTimespec("mainEventManager.Wait returned");

        libBLEEP_BL::AsyncEvent event = libBLEEP_BL::g_mainEventManager->PopAsyncEvent();
        
        switch (event.GetType()) {
        case libBLEEP_BL::AsyncEventEnum::Layer1_Event_Start ... libBLEEP_BL::AsyncEventEnum::Layer1_Event_End:
            libBLEEP_BL::g_SocketLayer_API->SwitchAsyncEventHandler(event);
            break;
        }
    }
}
