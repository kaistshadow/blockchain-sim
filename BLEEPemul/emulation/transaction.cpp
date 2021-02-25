// #include <stdlib.h>
#include <curl/curl.h>
#include <jsoncpp/json/json.h>
#include <string>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "rpc_client.h"

void rpc_getnewaddress(std::string wallet, char* ipport) {
    Json::Value params;
    params.clear();

    params = Json::arrayValue;
    params.append(wallet);
    bitcoin_rpc_request("setgeneratetoaddress",params);
}

void rpc_sendtoaddress(std::string IP, std::string wallet, int i, int amount) {

    std::cout<<i<<" transaction : \n";

    Json::Value params;
    params.clear();

    params = Json::arrayValue;
    params.append(wallet);
    params.append(amount);
    bitcoin_rpc_request("sendtoaddress",params);
}

int main(int argc, char* argv[]) {

    int interval = atoi(argv[3]);
    int txcnt = atoi(argv[2]);
    int amount = atoi(argv[4]);
    std::cout<<"start client \n";
    url = std::string(argv[1]);
    id_pwd = "a:1234";
    Json::Value params;
    params.clear();

    // method 1: generate node's wallet
    params = Json::arrayValue;
    bitcoin_rpc_request("getnewaddress", params);
    std::cout<<"-- wallet --\n";
    std::string wallet = json_resp["result"].asString();


    if(txcnt == -1 ) txcnt = 100000000000000;
    for(int i = 0; i <= txcnt; i++) {
        rpc_sendtoaddress(argv[1], wallet, i, amount);
        sleep(interval);
    }
    return 0;
}