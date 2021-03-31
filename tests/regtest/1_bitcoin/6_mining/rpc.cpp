#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <list>
#include <jsoncpp/json/json.h>
#include "../../../../testlibs/rpc_client.h"
#include "../../../../rpcAPI/rpc_request.h"

// param1 : IP address
// param2 : simulation time
// param3,4,5,6 ... : rpc functions
int main(int argc, char* argv[]) {
    
    std::string IP_address = argv[1];
    std::string sim_time = argv[2];
    std::string wallet;

    int i=0;
    int j=0;
    // Only receive function call
    // example) getnewaddress
    for(int i=0; i<argc; i++) {
        j = i;
        std::string str = argv[i];
        if(str.find("getnewaddress") != std::string::npos) {
            wallet = ReturnValue_call(str);
            break;
        }
    }

    // Only request function call more than 2 parameters.
    // example) setgeneratetoaddress
    for(int i=j; i<argc; i++) {
        j = i;
        std::string str = argv[i];
        if(str.find("setgeneratetoaddress") != std::string::npos) {
            Params_returnValue_call(str, wallet);
        }

    }


    // Only request function call.
    // example) getblockchaininfo
    for(int i=j; i<argc; i++) {

    }
}

// void rpc_generatetoaddress(std::string wallet, char* ipport) {
//     Json::Value params;
//     params.clear();

//     params = Json::arrayValue;
//     params.append(wallet);
//     bitcoin_rpc_request("setgeneratetoaddress",params);
// }

// void rpc_getblockchaininfo(){
//     Json::Value params;
//     params.clear();
//     bitcoin_rpc_request("getblockchaininfo",params);
// }

// void rpc_getmempoolinfo(){
//     Json::Value params;
//     params.clear();
//     bitcoin_rpc_request("getmempoolinfo",params);
// }

// int main(int argc, char* argv[]) {
//     // init
//     std::cout<<"start client \n";
//     url = std::string(argv[1]);
//     id_pwd = "a:1234";
//     Json::Value params;
//     params.clear();

//     // method 1: generate node's wallet
//     params = Json::arrayValue;
//     bitcoin_rpc_request("getnewaddress", params);
//     std::string wallet = json_resp["result"].asString();
//     std::cout<<"wallet:";
//     std::cout<<wallet;
//     std::cout<<"\n";

//     params.clear();
//     params = Json::arrayValue;
//     rpc_validateaddress(wallet, argv[1]);

//     rpc_generatetoaddress(wallet, argv[1]);

//     sleep(atoi(argv[2]));
//     rpc_getmempoolinfo();
//     rpc_getblockchaininfo();
//     return 0;
// }
