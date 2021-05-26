// #include <stdlib.h>
#include <curl/curl.h>
#include <jsoncpp/json/json.h>
#include <string>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "rpc_client.h"

int main(int argc, char* argv[]) {

    int interval = atoi(argv[3]);
    int txcnt = atoi(argv[2]);
    url = std::string(argv[1]);
    std::string amount = argv[4];
    id_pwd = "a:1234";
    Json::Value params;
    params.clear();

    // method 1: generate node's wallet
    params = Json::arrayValue;
    std::string wallet = rpc_request_with_no_params("getnewaddress");
    std::cout<<"wallet:";
    std::cout<<wallet;
    std::cout<<"\n";

    int i=0;
    std::list<std::string> params_list;
    while(1)
    {
        sleep(interval);
        params_list.push_front(wallet);
        params_list.push_front(amount);
        std::cout << i << " : ";
        rpc_reqeust_with_params("sendtoaddress", params_list);
        i++;
        params_list.clear();
    }
    return 0;
}

