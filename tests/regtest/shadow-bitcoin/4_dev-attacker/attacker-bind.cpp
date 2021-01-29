//
// Created by ilios on 20. 9. 11..
//

#include "BL_MainEventManager.h"
#include "BL1_socket/SocketLayer_Bitcoin.h"
#include "BL1_socket/SocketLayer_API.h"


#include "utility/ArgsManager.h"
#include "utility/Logger.h"

#include <algorithm>
#include <string_view>

#include "shadow_interface.h"
#include <arpa/inet.h>
#include <fcntl.h> /* Added for the nonblocking socket */

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

    std::cout << "main started for attacker-bind" << "\n";

    puts_temp("test shadow_interface\n");

//    int new_server_fd = socket(AF_INET, SOCK_STREAM, 0);
//
//    int 			new_bindfd;
//    struct 	sockaddr_in 	my_addr;    /* my address information */
//    if ((new_bindfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
//        perror("socket");
//        exit(1);
//    }
//    my_addr.sin_family = AF_INET;         /* host byte order */
//    my_addr.sin_port = htons(8333);     /* short, network byte order */
////    my_addr.sin_addr.s_addr = INADDR_ANY; /* auto-fill with my IP */
//    my_addr.sin_addr.s_addr = inet_addr("11.0.0.10");
//    bzero(&(my_addr.sin_zero), 8);        /* zero the rest of the struct */
//
//    if (shadow_bind(new_bindfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
//        perror("bind");
//        exit(1);
//    }

    /* allocate mainEventManager */
    libBLEEP_BL::MainEventManager::InitInstance();

    /* allocate socketlayer */
    libBLEEP_BL::g_SocketLayer_API = std::unique_ptr<libBLEEP_BL::BL_SocketLayer_API>(
            new libBLEEP_BL::BL_SocketLayer_Bitcoin());
    /* allocate peerConnectivityLayer */
    //    std::string myId = gArgs.GetArg("-id", "noid");
    //    libBLEEP_BL::g_PeerConnectivityLayer_API = std::unique_ptr<libBLEEP_BL::BL_PeerConnectivityLayer_API>(new libBLEEP_BL::BL_PeerConnectivityLayer(myId));

    libBLEEP_BL::g_SocketLayer_API->CreateSocketForShadowIP(DEFAULT_SOCKET_PORT, "11.0.0.10");


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
            case libBLEEP_BL::AsyncEventEnum::BitcoinRecvMsg:
                std::string bitcoinCommand = event.GetData().GetBitcoinCommand();
                uint32_t payloadSize = event.GetData().GetBitcoinPayloadLen();
                uint32_t checksum = event.GetData().GetBitcoinPayloadChecksum();
                std::string bitcoinPayload = event.GetData().GetBitcoinPayload();
                std::cout << "-------------------------------------------MSG " << ++bitcoin_msg_count << "------------------------------------------\n";
                std::cout << "received bitcoin command : ";
                std::cout << bitcoinCommand << "\n";
                std::cout << "received bitcoin command (hex) : ";
                std::cout << string_to_hex(bitcoinCommand) << "\n";

                std::cout << "received bitcoin payload : ";
                std::cout << bitcoinPayload << "\n";
                std::cout << "received bitcoin payload (hex) : ";
                std::cout << string_to_hex(bitcoinPayload) << "\n";
                std::cout << "------------------------------------------------------------------------------------------\n";

                int fd = event.GetData().GetBitcoinRecvSocket();

                auto bitCommandView = std::string_view(bitcoinCommand);
                if (bitCommandView.find("version") == 0) {
                    // if it received version message from a victim, send a valid version reply.
                    std::string response_hex = "f9beb4d976657273696f6e000000000066000000dad86bee7f11010009040000000000000de10b5e00000000000000000000000000000000000000000000ffff0b000002a917090400000000000000000000000000000000000000000000000099783b985a421805102f5361746f7368693a302e31392e312f0000000001f9beb4d976657261636b000000000000000000005df6e0e2";
                    std::string response = hex_to_string(response_hex);
                    libBLEEP_BL::g_SocketLayer_API->SendToSocket(fd, response.c_str(), response.size());
                }

                break;
        }
    }
}
