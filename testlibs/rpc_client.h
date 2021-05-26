/*
 *
 *  2021 02 18
 *  Created by Hyunjin Kim
 *
 */

#ifndef BLEEP_RPC_CLIENT_H_
#define BLEEP_RPC_CLIENT_H_

// #include "./../Install/curl_7.70.0/include/curl/curl.h"
#include <curl/curl.h>
#include <jsoncpp/json/json.h>
#include <string>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <list>

// json reader
extern Json::Value json_req;
extern Json::Value json_resp;
// bitcoin rpc curl library
extern std::string url;
extern std::string id_pwd;
struct response {
    char* data;
    size_t size;
};

size_t rpc_callback(char *data, size_t size, size_t nmemb, void *userdata);
std::string request(std::string req);

void bitcoin_rpc_request(std::string method, Json::Value params) ;
void rpc_reqeust_with_params(std::string rpc_function, std::list<std::string> params_list);
std::string rpc_request_with_no_params(std::string rpc_function);
void rpc_request_no_return_no_params(std::string rpc_function);

#endif