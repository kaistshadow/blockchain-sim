#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <jsoncpp/json/json.h>
#include "../../../../testlibs/rpc_client.h"

int main(int argc, char* argv[]) {
    // init
    std::cout<<"start client \n";
    url = std::string(argv[1]);
    id_pwd = "a:1234";
    Json::Value params;
    params.clear();

    // method 1: generate node's wallet
    params = Json::arrayValue;
    std::string wallet = rpc_request_with_no_params("getnewaddress");
    std::cout<<"wallet:";
    std::cout<<wallet;
    std::cout<<"\n";

    params.clear();
    params = Json::arrayValue;
    std::list<std::string> params_list;
    params_list.push_front(wallet);

    rpc_reqeust_with_params("validateaddress", params_list);
    rpc_reqeust_with_params("setgeneratetoaddress", params_list);
    sleep(atoi(argv[2]));
    rpc_request_no_return_no_params("getmempoolinfo");
    rpc_request_no_return_no_params("getblockchaininfo");
    return 0;
}
