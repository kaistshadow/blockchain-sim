/*

This is a test to see if custom curl works in bitcoin after testing for custom url.

Input parameter : Anything bitcoin-rpc functions
Output : The returnd data from bitcoin-rpc function you set

*/
#include <stdio.h>
#include <curl/curl.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

int main(void)
{
    printf("start\n");

    CURL *curl;
    CURLcode res;
    char *host = "http://213.145.108.101:18332"; //127.0.0.1:18332 213.145.108.101:18332

    curl = curl_easy_init();
    struct curl_slist *headers = NULL;

    if (curl) {
	const char *data ="{\"jsonrpc\": \"1.0\", \"id\":\"curltest\", \"method\": \"getblockchaininfo\", \"params\": [] }";

	headers = curl_slist_append(headers, "content-type: text/plain;");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	curl_easy_setopt(curl, CURLOPT_URL, host);

	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long) strlen(data));
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);

	curl_easy_setopt(curl, CURLOPT_USERPWD, "node1:1234");
	curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_TRY);

    printf("step1\n");
    res = curl_easy_perform(curl);
    printf("step2\n");

    /* Check for errors */
    while(res != CURLE_OK) {
        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
        else
            printf("success\n");
        sleep(1);
    }
 
    /* always cleanup */ 
    curl_easy_cleanup(curl);  
      }

    printf("step3\n");
    return 0;
}
