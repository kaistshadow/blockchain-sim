//
// Created by ilios on 20. 9. 11..
//

#include "BL_MainEventManager.h"
#include "BL1_socket/SocketLayer_Bitcoin.h"
#include "BL1_socket/SocketLayer_API.h"
#include "BL1_socket/Socket.h"


#include "utility/ArgsManager.h"
#include "utility/Logger.h"

#include <algorithm>
#include <string_view>

#include "shadow_interface.h"
#include <arpa/inet.h>

std::string string_to_hex(const std::string& input)
{
    static const char hex_digits[] = "0123456789ABCDEF";

    std::string output;
    output.reserve(input.length() * 2);
    for (unsigned char c : input)
    {
        output.push_back(hex_digits[c >> 4]);
        output.push_back(hex_digits[c & 15]);
    }
    return output;
}
// Convert string of hex numbers to its equivalent char-stream
std::string hex_to_string(const std::string& hexstr)
{
    std::string output;
    output.resize((hexstr.size() + 1) / 2);

    for (size_t i = 0, j = 0; i < output.size(); i++, j++)
    {
        output[i] = (hexstr[j] & '@' ? hexstr[j] + 9 : hexstr[j]) << 4, j++;
        output[i] |= (hexstr[j] & '@' ? hexstr[j] + 9 : hexstr[j]) & 0xF;
    }
    return output;
}

using namespace libBLEEP;


int bitcoin_msg_count = 0;

int main(int argc, char *argv[]) {

    gArgs.ParseParameters(argc, argv);

    if (gArgs.IsArgSet("-?") || gArgs.IsArgSet("-h") ||  gArgs.IsArgSet("-help") || gArgs.IsArgSet("-version")) {
        std::string strUsage = gArgs.HelpMessage();
        gLog << strUsage << "\n";
        return 0;
    }

    std::cout << "main started for sender-bind" << "\n";

    puts_temp("test shadow_interface\n");

    /* allocate mainEventManager */
    libBLEEP_BL::MainEventManager::InitInstance();

    /* allocate socketlayer */
    libBLEEP_BL::g_SocketLayer_API = std::unique_ptr<libBLEEP_BL::BL_SocketLayer_API>(
            new libBLEEP_BL::BL_SocketLayer_Bitcoin());
    /* allocate peerConnectivityLayer */
//    std::string myId = gArgs.GetArg("-id", "noid");
//    libBLEEP_BL::g_PeerConnectivityLayer_API = std::unique_ptr<libBLEEP_BL::BL_PeerConnectivityLayer_API>(new libBLEEP_BL::BL_PeerConnectivityLayer(myId));


    int conn_fd = libBLEEP_BL::g_SocketLayer_API->ConnectSocket("11.0.0.10");

    while(true) {
        std::cout << "while" << "\n";
        libBLEEP_BL::MainEventManager::Instance()->Wait(); // main event loop (wait for next event)

        // loop returned
        PrintTimespec("mainEventManager.Wait returned");

        libBLEEP_BL::AsyncEvent event = libBLEEP_BL::MainEventManager::Instance()->PopAsyncEvent();

        switch (event.GetType()) {
            case libBLEEP_BL::AsyncEventEnum::Layer1_Event_Start ... libBLEEP_BL::AsyncEventEnum::Layer1_Event_End:
                libBLEEP_BL::g_SocketLayer_API->SwitchAsyncEventHandler(event);
                break;
            case libBLEEP_BL::AsyncEventEnum::PeerSocketConnect:
                std::shared_ptr<libBLEEP_BL::DataSocket> dataSocket = event.GetData().GetDataSocket();
                int fd = dataSocket->GetFD();
                std::string test = "test message for newly binded socket!!";
                libBLEEP_BL::g_SocketLayer_API->SendToSocket(fd, test.c_str(), test.size());
                break;
        }
    }
}
