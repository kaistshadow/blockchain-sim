#include <curl/curl.h>
#include <jsoncpp/json/json.h>
#include <string>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <list>
#include <tuple>
#include "rpc_client.h"
#include "rpc_request.h"

auto filter_rpc_functions(int rpc_count, char* argv[]) {
    
    std::list<std::string> receive_list; // getnewaddress
    std::list<std::string> request_list; // setgeneratetoaddress, validateaddress, sendtoaddress
    std::list<std::string> send_list; // getblockchaininfo, getmempoolinfos, listaddressgroupings, rpc_getpeerinfo

    for(int i=0; i<rpc_count; i++) {
        std::string str = argv[i];
        if(str.find("_1") != std::string::npos) {
            receive_list.push_back(str);
            continue;
        }
        if(str.find("_2") != std::string::npos) {
            request_list.push_back(str);
            continue;
        }
        if(str.find("_3") != std::string::npos) {
            send_list.push_back(str);
            continue;
        }
    }
    std::tuple <std::list<std::string>, std::list<std::string>, std::list<std::string>> return_list = make_tuple(receive_list, request_list, send_list);

    return return_list;
}

void No_returnValue_call(std::string rpc_request) {
    Json::Value params;
    params.clear();
    bitcoin_rpc_request(rpc_request, params);
}

std::string ReturnValue_call(std::string rpc_request) {
    Json::Value params;
    params.clear();
    bitcoin_rpc_request(rpc_request, params);
    std::string rpc_output = json_resp["result"].asString();
    std::cout<<rpc_output<<std::endl;
    return rpc_output;
}

void Params_returnValue_call(std::string rpc_request, std::string inputParams) {
    Json::Value params;
    params.clear();
    params = Json::arrayValue;
    params.append(inputParams);
    bitcoin_rpc_request(rpc_request, params);
    std::string rpc_output = json_resp["result"].asString();
    std::cout<<rpc_output<<std::endl;
}

void returnValue_use_call(std::string rpc_request, std::string rpc_request2, std::string inputParams) {
    Json::Value params;
    params.clear();
    params = Json::arrayValue;
    params.append(inputParams);
    bitcoin_rpc_request(rpc_request, params);
    std::string rpc_output = json_resp["result"].asString();
    Params_returnValue_call()
}