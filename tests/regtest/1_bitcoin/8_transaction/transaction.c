// #include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <stdio.h>
#include <unistd.h>

struct string {
    char *ptr;
    size_t len;
};

void init_string(struct string *s) {
    s->len = 0;
    s->ptr = malloc(s->len+1);
    if (s->ptr == NULL) {
        fprintf(stderr, "malloc() failed\n");
        exit(EXIT_FAILURE);
    }
    s->ptr[0] = '\0';
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s)
{
    size_t new_len = s->len + size*nmemb;
    s->ptr = realloc(s->ptr, new_len+1);
    if (s->ptr == NULL) {
        fprintf(stderr, "realloc() failed\n");
        exit(EXIT_FAILURE);
    }
    memcpy(s->ptr+s->len, ptr, size*nmemb);
    s->ptr[new_len] = '\0';
    s->len = new_len;
    return size*nmemb;
}

void getWalletaddress(char* wallet, char* string_info) {
    for (int i = 11; i < 45; i++) { // 46
        wallet[i - 11] = string_info[i];
    }
}

void rpc_getnewaddress(char* wallet, char* ipport) {
    CURL *curl = curl_easy_init();
    struct curl_slist *headers = NULL;
    if (curl) {
        struct string s;
        init_string(&s);
        const char *getnewaddress = "{\"jsonrpc\": \"1.0\", \"id\":\"curltest\", \"method\": \"getnewaddress\", \"params\": [] }";
        headers = curl_slist_append(headers, "content-type: text/plain;");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_URL, ipport);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long) strlen(getnewaddress));
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, getnewaddress);
        curl_easy_setopt(curl, CURLOPT_USERPWD,"a:1234");
        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_TRY);
        curl_easy_perform(curl);
        getWalletaddress(wallet, s.ptr);
        free(s.ptr);
    }
}

void rpc_sendtoaddress(char* IP, char* wallet, int i) {

    char first[100] = "{\"jsonrpc\": \"1.0\", \"id\":\"curltest\", \"method\": \"sendtoaddress\", ";
    char input[250];
    char second[30] = "\"params\": [";
    char last[50];
    strcpy(input, first);
    strcat(input, second);
    sprintf(last, "\"%s\"", wallet);
    strcat(input, last);
    strcat(input, ", 0.01 ]}");
    printf("%d transaction : ", i);
    CURL *curl = curl_easy_init();
    struct curl_slist *headers = NULL;

    if (curl) {

        headers = curl_slist_append(headers, "content-type: text/plain;");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_URL, IP);


        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long) strlen(input));
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, input);

        curl_easy_setopt(curl, CURLOPT_USERPWD,
                         "a:1234");

        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_TRY);

        curl_easy_perform(curl);
    }
}

int main(int argc, char* argv[]) {
    int i =0;
    char wallet[36];
    memset(wallet, 0, sizeof(char)*36);
    rpc_getnewaddress(wallet, argv[1]);
    while(1) {
        i += 1;
        rpc_sendtoaddress(argv[1], wallet, i);
        sleep(2);
    }
    return 0;
}
