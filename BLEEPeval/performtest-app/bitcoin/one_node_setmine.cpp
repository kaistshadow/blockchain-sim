#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <jsoncpp/json/json.h>
#include "../../../testlibs/rpc_client.h"

int main(int argc, char* argv[]) {
    // init
    std::cout<<"start client \n";
    url = std::string(argv[1]);
    id_pwd = "a:1234";
    Json::Value params;
    params.clear();

    // method 1: generate node's wallet
    std::string wallet = rpc_request_with_no_params("getnewaddress");
    std::cout<<"wallet:" <<wallet <<"\n";
    params.append(wallet);

    bitcoin_rpc_request("setgeneratetoaddress", params);
    
    return 0;
}