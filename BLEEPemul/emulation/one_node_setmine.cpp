#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <jsoncpp/json/json.h>
#include "rpc_client.h"

void rpc_generatetoaddress(std::string wallet, char* ipport) {
    Json::Value params;
    params.clear();

    params = Json::arrayValue;
    params.append(wallet);
    bitcoin_rpc_request("setgeneratetoaddress",params);
}

void rpc_getblockchaininfo(char* ipport){
    const char *data ="{\"jsonrpc\": \"1.0\", \"id\":\"curltest\", \"method\": \"getblockchaininfo\", \"params\": [] }";
    CURL *curl = curl_easy_init();
    struct curl_slist *headers = NULL;
    if (curl) {
        headers = curl_slist_append(headers, "content-type: text/plain;");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_URL, ipport);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long) strlen(data));
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
        curl_easy_setopt(curl, CURLOPT_USERPWD, "a:1234");
        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_TRY);
        curl_easy_perform(curl);
    }
}
void rpc_getmempoolinfo(char* ipport){
    const char *data ="{\"jsonrpc\": \"1.0\", \"id\":\"curltest\", \"method\": \"getmempoolinfo\", \"params\": [] }";
    CURL *curl = curl_easy_init();
    struct curl_slist *headers = NULL;
    if (curl) {
        headers = curl_slist_append(headers, "content-type: text/plain;");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_URL, ipport);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long) strlen(data));
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
        curl_easy_setopt(curl, CURLOPT_USERPWD, "a:1234");
        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_TRY);
        curl_easy_perform(curl);
    }
}
void make100blocks(std::string wallet_address) {
    Json::Value params;
    params.append(wallet_address);
    for (int i = 0; i < 101; i++) {
        std::cout<<i;
        bitcoin_rpc_request("setgeneratetoaddressnotime", params);
        std::cout << "\n";
    }
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

    make100blocks(wallet);
    rpc_generatetoaddress(wallet, argv[1]);

    sleep(atoi(argv[2]));
    rpc_getmempoolinfo(argv[1]);
    return 0;
}
