// #include <stdlib.h>
#include <jsoncpp/json/json.h>
#include <string>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "../../testlibs/rpc_client.h"

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
    bitcoin_rpc_request("listaddressgroupings", params);
    std::cout<<"-- wallet --\n";
    std::string wallet = json_resp["result"][0][0][0].asString();
    std::cout<<wallet<<"\n";
    sleep(1);
    int blockcnt=0;
    while(blockcnt <= 6) {
        rpc_request_no_return_no_params("getblockchaininfo");
        blockcnt = json_resp["result"]["blocks"].asInt();
        sleep(1);
    }

    int i=0;
    std::list<std::string> params_list;

    if(txcnt == -1) {
        while(1) {
            params_list.push_front(amount);
            params_list.push_front(wallet);
            rpc_reqeust_with_params("sendtoaddress", params_list);
            sleep(0.5);
            i++;
            params_list.clear();
        }
    } else {
        for(i=0; i<txcnt; i++) {
            params_list.push_front(amount);
            params_list.push_front(wallet);
            rpc_reqeust_with_params("sendtoaddress", params_list);
            sleep(interval);
            params_list.clear();
        }
    }
    return 0;
}