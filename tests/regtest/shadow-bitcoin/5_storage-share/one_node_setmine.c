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
    for (int i = 11; i < 46; i++) { // 46
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
        printf("%s \n", wallet);
        getWalletaddress(wallet, s.ptr);
        free(s.ptr);
    }
}

void rpc_generatetoaddress(char* wallet, char* ipport) {
    char input[250];
    char first[100] = "{\"jsonrpc\": \"1.0\", \"id\":\"curltest\", \"method\": \"setgeneratetoaddress\", ";
    char second[30] = "\"params\": [";
    char last[50];
    strcpy(input, first);
    strcat(input, second);
    sprintf(last, "\"%s\"", wallet);
    strcat(input, last);
    strcat(input, "]}");
    printf("%s \n", input);
    CURL *curl = curl_easy_init();
    struct curl_slist *headers = NULL;
    if (curl) {
        headers = curl_slist_append(headers, "content-type: text/plain;");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_URL, ipport);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long) strlen(input));
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, input);
        curl_easy_setopt(curl, CURLOPT_USERPWD, "a:1234");
        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_TRY);
        curl_easy_perform(curl);
    }
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
void parsing_txcount(char* nodename) {
    char ptr[30];
    strcpy(ptr,nodename);
    char *nodeid = strtok(ptr,"e");
    nodeid=strtok(NULL,"");
    char input[100] = "";
    char str[4][20] = {"./datadir/hosts/","/stdout-",".bitcoind.",".log"};

    strcat(input, str[0]);
    strcat(input,nodename);
    strcat(input, str[1]);
    strcat(input,nodename);
    strcat(input,str[2]);
    strcat(input,nodeid);
    strcat(input, str[3]);

    FILE *inputFile = fopen(input,"r");
    int txcnt=0;
    if (inputFile){
        while(!feof(inputFile)){
            char buf[256];
            fgets(buf,sizeof(buf),inputFile);
            char *ptr = strtok(buf," ");
            if(ptr){
                ptr=strtok(NULL," ");
                if(ptr){
                    if(memcmp(ptr,"WriteBlockToDisk-txCount",sizeof("WriteBlockToDisk-txCount"))==0) {
                        ptr = strtok(NULL, " ");
                        txcnt+=atoi(ptr);
                    }
                    else continue;
                }
            }
        }
        printf("{\"tx Count: %d\"}\n",txcnt);
        fclose(inputFile);
    }
}

int main(int argc, char* argv[]) {
    char wallet[36];
    memset(wallet, 0, sizeof(char)*36);
    rpc_getnewaddress(wallet, argv[1]);
    rpc_generatetoaddress(wallet, argv[1]);
    sleep(atoi(argv[2]));
    rpc_getblockchaininfo(argv[1]);
//    parsing_txcount(argv[3]);
    return 0;
}
