#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <jsoncpp/json/json.h>
#include "../../testlibs/rpc_client.h"

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
    std::cout<<"-- wallet --\n";
    std::string wallet = json_resp["result"].asString();

    params.clear();
    params.append(wallet);
    for(int i=0;i<7;i++){
      bitcoin_rpc_request("setgeneratetoaddressnotime",params);
    }

    bitcoin_rpc_request("dumpprivkey",params);

    params.clear();
    params.append(1);
    bitcoin_rpc_request("getblockhash",params);
    std::string block1_hash = json_resp["result"].asString();

    params.clear();
    params.append(block1_hash);
    params.append(true);
    bitcoin_rpc_request("getblock",params);
    std::string coinbase_txhash = json_resp["result"]["tx"][0].asString();
    std::cout<<"height 1 coinbase txhash = " <<coinbase_txhash<<"\n";

    params.clear();
    params.append(coinbase_txhash);
    params.append(true);
    bitcoin_rpc_request("getrawtransaction",params);

    sleep(atoi(argv[2]));
    rpc_getmempoolinfo();
    rpc_getblockchaininfo();
    return 0;
}