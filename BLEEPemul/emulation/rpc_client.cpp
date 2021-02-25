/*
 *
 *  2021 02 18
 *  Created by Hyunjin Kim
 *
 */

#include <curl/curl.h>
#include <jsoncpp/json/json.h>
#include <string>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

// json reader
Json::Reader reader;
Json::FastWriter writer;
Json::Value json_req;
Json::Value json_resp;
// bitcoin rpc curl library
std::string url;
std::string id_pwd;
struct response {
    char* data;
    size_t size;
};
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
        std::cout<<"-- result --\n";
        std::cout<<res;
        return res;
    }
    return "";
}
void init_fixed_attr(int argc, char* argv[]) {
    if (argc != 4) {
        std::cout<<"argument not match\n";
        abort();
    }
    url = std::string(argv[1]);
    id_pwd = std::string(argv[2]);
}

// feature supports
int getTxCount(std::string str) {
    std::size_t found;
    found = str.find("\"result\":");
    std::string s = str.substr(found + 9, str.size());
    found = s.find(",");
    return atoi(s.substr(0, found).c_str());
}


int main(int argc, char* argv[]) {
    // init
    std::string req;
    std::string res;
    init_fixed_attr(argc, argv);

    // method 1: generate node's wallet
    std::cout<<"getnewaddress\n";
    json_req.clear();
    json_resp.clear();
    // request setup
    json_req["jsonrpc"] = "1.0";
    json_req["id"] = "rpc_client";
    json_req["method"] = "getnewaddress";
    json_req["params"] = Json::arrayValue;
    // request
    req = writer.write(json_req);
    res = request(req);
    reader.parse(res, json_resp);
    std::cout<<"-- wallet --\n";
    std::string wallet_address = json_resp["result"].asString();
    std::cout<<wallet_address<<"\n";
    std::cout<<"\n";

    // method 2: setgeneratetoaddress
    std::cout<<"setgeneratetoaddress\n";
    json_req.clear();
    json_resp.clear();
    // request setup
    json_req["jsonrpc"] = "1.0";
    json_req["id"] = "rpc_client";
    json_req["method"] = "setgeneratetoaddress";
    json_req["params"] = Json::arrayValue;
    json_req["params"].append(wallet_address);
    // request
    req = writer.write(json_req);
    res = request(req);
    std::cout<<"\n";

    sleep(atoi(argv[3]));

    // method 3: getblockchaininfo
    std::cout<<"getblockchaininfo\n";
    json_req.clear();
    json_resp.clear();
    // request setup
    json_req["jsonrpc"] = "1.0";
    json_req["id"] = "rpc_client";
    json_req["method"] = "getblockchaininfo";
    json_req["params"] = Json::arrayValue;
    // request
    req = writer.write(json_req);
    res = request(req);
    std::cout<<"\n";

    // method 4: gettxtotalcount
    std::cout<<"gettxtotalcount\n";
    json_req.clear();
    json_resp.clear();
    // request setup
    json_req["jsonrpc"] = "1.0";
    json_req["id"] = "rpc_client";
    json_req["method"] = "gettxtotalcount";
    json_req["params"] = Json::arrayValue;
    // request
    req = writer.write(json_req);
    res = request(req);
    reader.parse(res, json_resp);
    std::cout<<"-- TPS --\n";
    int txcnt = json_resp["result"].asInt();
    std::cout<<txcnt<<"/"<<argv[3]<<"="<<(txcnt/(double)atoi(argv[3]))<<"\n";
    std::cout<<"\n";
}