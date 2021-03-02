#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <jsoncpp/json/json.h>
#include "../libraries/rpc_client.h"

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

void make100blocks(std::string wallet_address) {
    Json::Value params;
    params.append(wallet_address);
    for (int i = 0; i < 101; i++) {
        std::cout<<i;
        bitcoin_rpc_request("setgeneratetoaddressnotime", params);
        std::cout << "\n";
    }
}

int main(int argc, char* argv[]) {
    // init
    std::cout<<"start client \n";
    url = std::string(argv[1]);
    id_pwd = "a:1234";
    Json::Value params;
    params.clear();

    rpc_getblockchaininfo();
    return 0;
}
