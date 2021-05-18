/*
 *
 *  2021 02 18
 *  Created by Hyunjin Kim
 *
 */

#include "./../Install/curl_7.70.0/include/curl/curl.h"
#include <jsoncpp/json/json.h>
#include <string>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <list>
#include "rpc_client.h"

Json::Reader reader;
Json::FastWriter writer;
Json::Value json_req;
Json::Value json_resp;
// bitcoin rpc curl library
std::string url="";
std::string id_pwd="";
size_t rpc_callback(char *data, size_t size, size_t nmemb, void *userdata) {
    size_t realsize = size * nmemb;

    struct response *res = (struct response *)userdata;

    char *ptr = (char*)realloc(res->data, res->size + realsize + 1);
    if(ptr == NULL)
        return 0;  /* out of memory! */

    res->data = ptr;
    memcpy(&(res->data[res->size]), data, realsize);
    res->size += realsize;
    res->data[res->size] = 0;

    return realsize;
}

std::string request(std::string req){
    std::cout<<"request : " <<req.c_str();
    CURL *curl = curl_easy_init();
    struct curl_slist *headers = NULL;
    struct response r = {nullptr, 0};
    if (curl) {
        headers = curl_slist_append(headers, "content-type: text/plain;");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, rpc_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&r);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, req.size());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, req.c_str());
        curl_easy_setopt(curl, CURLOPT_USERPWD, id_pwd.c_str());
        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_TRY);
        curl_easy_perform(curl);

        std::string res = std::string(r.data, r.size);
        free(r.data);
        std::cout<<"-- result : "<<res<<"\n";
        return res;
    }
    return "";
}

void bitcoin_rpc_request(std::string method, Json::Value params) {
    json_req.clear();
    json_resp.clear();
    // request setup
    json_req["jsonrpc"] = "1.0";
    json_req["id"] = "rpc_client";
    json_req["method"] = method;
    json_req["params"] = params;
    // request
    std::string req = writer.write(json_req);
    std::string res = request(req);
    reader.parse(res, json_resp);
}

// example : setgeneratetoaddress rpc function
// This function is used when calling rpc that requires parameters.
void rpc_reqeust_with_params(std::string rpc_function, std::list<std::string> params_list) {
    Json::Value params;
    params.clear();
    params = Json::arrayValue;
    for(auto const& i: params_list) {
        params.append(i);
    }
    bitcoin_rpc_request(rpc_function, params);
}

// example : getnewaddress rpc function
// To get wallet address from "getnewaddress" rpc function call
// Like this, this function enable to get string data from rpc call 
std::string rpc_request_with_no_params(std::string rpc_function) {
    Json::Value params;
    params.clear();
    params = Json::arrayValue;
    bitcoin_rpc_request(rpc_function, params);
    std::string return_value = json_resp["result"].asString();
    return return_value;
}

// example : getblockchaininfo rpc function
// This function is used when no params, no return values.
void rpc_request_no_return_no_params(std::string rpc_function) {
    Json::Value params;
    params.clear();
    params = Json::arrayValue;
    bitcoin_rpc_request(rpc_function, params);
}
