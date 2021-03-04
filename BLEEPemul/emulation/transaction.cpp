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

void rpc_sendtoaddress(std::string IP, std::string wallet, int i, std::string amount) {

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
    std::string amount = argv[4];
    std::cout<<"start client \n";
    url = std::string(argv[1]);
    id_pwd = "a:1234";
    Json::Value params;
    params.clear();

    // method 1: generate node's wallet
    std::cout<<"-- wallet --\n";
    bitcoin_rpc_request("getnewaddress", params);
    std::string wallet = json_resp["result"].asString();

    sleep(1);
    int blockcnt=0;
    while(blockcnt <= 6) {
        bitcoin_rpc_request("getblockchaininfo", params);
        blockcnt = json_resp["result"]["blocks"].asInt();
        sleep(1);
    }

    if(txcnt == -1 ){
        int i=0;
        while(1) {
            rpc_sendtoaddress(argv[1],wallet, i, amount);
            sleep(0.1);
            i++;
        }
    } else {
        for(int i = 0; i <= txcnt; i++) {
            rpc_sendtoaddress(argv[1], wallet, i, amount);
            sleep(interval);
        }
    }
    return 0;
}