

#ifndef RPC_REQUEST_API
#define RPC_REQUEST_API

#include <curl/curl.h>
#include <jsoncpp/json/json.h>
#include <string>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <list>
#include <stdlib.h>
#include "rpc_client.h"

auto filter_rpc_functions(int rpc_count, char* argv[]);
void No_returnValue_call(std::string rpc_request);
void Params_no_returnValue_call(std::string rpc_request, std::string params);
std::string ReturnValue_call(std::string rpc_request);
void Params_returnValue_call(std::string rpc_request, std::string inputParams);

#endif
