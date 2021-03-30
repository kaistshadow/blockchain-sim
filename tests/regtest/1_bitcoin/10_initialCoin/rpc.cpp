#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <jsoncpp/json/json.h>
#include "../../../../testlibs/rpc_client.h"

void rpc_generatetoaddress(std::string wallet, char* ipport) {
    Json::Value params;
    params.clear();

    params = Json::arrayValue;
    params.append(wallet);
    bitcoin_rpc_request("setgeneratetoaddress",params);
}

void rpc_getblockchaininfo(){
    Json::Value params;
    params.clear();
    bitcoin_rpc_request("getblockchaininfo",params);
}

void rpc_getmempoolinfo(){
    Json::Value params;
    params.clear();
    bitcoin_rpc_request("getmempoolinfo",params);
}

int main(int argc, char* argv[]) {
    // init
    std::cout<<"start client \n";
    url = std::string(argv[1]);
    id_pwd = "a:1234";
    Json::Value params;
    params.clear();

    // method 1: generate node's wallet
    params = Json::arrayValue;
    bitcoin_rpc_request("getnewaddress", params);
    std::string wallet = json_resp["result"].asString();
    std::cout<<"wallet:";
    std::cout<<wallet;
    std::cout<<"\n";

    params.clear();
    params = Json::arrayValue;
    rpc_validateaddress(wallet, argv[1]);
    rpc_generatetoaddress(wallet, argv[1]);

    sleep(40);
    rpc_getblockchaininfo();
    return 0;
}
