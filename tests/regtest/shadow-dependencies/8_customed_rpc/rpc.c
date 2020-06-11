// #include <stdlib.h>
#include <stdio.h>
#include <curl/curl.h>
#include <arpa/inet.h>
#include <netdb.h>

int main(void)
{
    printf("start\n");

    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    struct curl_slist *headers = NULL;

    if (curl) {
	const char *data ="{\"jsonrpc\": \"1.0\", \"id\":\"curltest\", \"method\": \"getblockchaininfo\", \"params\": [] }";


	struct addrinfo* servinfo;
    int n = getaddrinfo(domain.c_str(), NULL, NULL, &servinfo);
    if (n != 0) {
        std::cout << "Value of errno: " << errno << "\n";
        fprintf(stderr, "getaddrinfo:%s\n", gai_strerror(n));
        perror("getaddrinfo");
        std::cout << "domain" << domain << "\n";
    }


	headers = curl_slist_append(headers, "content-type: text/plain;");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	// curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:11111");
	curl_easy_setopt(curl, CURLOPT_URL, "https://bcdnode1");

	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long) strlen(data));
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);

	curl_easy_setopt(curl, CURLOPT_USERPWD,
			 "a:1234");

	curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_TRY);
    printf("아하1\n");
    res = curl_easy_perform(curl);
    printf("아하2\n");

    /* Check for errors */ 
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
 
    /* always cleanup */ 
    curl_easy_cleanup(curl);  
      }

    printf("아하3\n");
    return 0;
}