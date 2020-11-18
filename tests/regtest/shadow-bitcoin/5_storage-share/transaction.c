/*
 *
 *  2020 07 10
 *  Created by Hong Joon
 *
 */

#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <stdio.h>
#include <time.h>

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
    int i = 0;
    char *ptr = strtok(string_info, "\":\"");

    while (ptr != NULL)
    {
        printf("%s\n", ptr);
        ptr = strtok(NULL, "\"\"");
        i ++;
        if(i == 3){
            break;
        }
    }
    strcpy(wallet, ptr);
}

void rpc_getnewaddress(char* IP, char* wallet){

    printf("-------------------------<<<<<<<<<<<<<<<<<<< start getnewaddress \n\n");
    printf("%s \n\n", IP);
    CURL *curl = curl_easy_init();
    struct curl_slist *headers = NULL;

    if (curl) {
        struct string s;
        init_string(&s);
        const char *getnewaddress =
                "{\"jsonrpc\": \"1.0\", \"id\":\"curltest\", \"method\": \"getnewaddress\", \"params\": [] }";

        headers = curl_slist_append(headers, "content-type: text/plain;");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_URL, IP);


        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long) strlen(getnewaddress));
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, getnewaddress);

        curl_easy_setopt(curl, CURLOPT_USERPWD,
                         "a:1234");

        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_TRY);

        curl_easy_perform(curl);
        printf(" string_info %s \n", s.ptr);
        getWalletaddress(wallet, s.ptr);
        printf(" wallet - %s \n", wallet);
        free(s.ptr);
    }
    printf("\n\n");
}

void rpc_sendtoaddress(char* IP, char* wallet) {

    printf("------------------------------------------------------------- start sendtoaddress \n\n");
    char first[100] = "{\"jsonrpc\": \"1.0\", \"id\":\"curltest\", \"method\": \"sendtoaddress\", ";
    char input[250];
    char second[30] = "\"params\": [";
    char last[50];
    strcpy(input, first);
    strcat(input, second);
    sprintf(last, "\"%s\"", wallet);
    strcat(input, last);
    strcat(input, ", 0.00001 ]}");
    printf("%s \n", input);

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

void set_IP_and_Address(char** IP_array, char** wallet_array, int end) {

    int j = 0;
    int i = 0;
    int z = 1;
    while(1) {
        if (i > end-1) {
            break;
        }

        char IP_format[20] = "\0";
        char IP_first_part[5] = "\0";
        char IP_second_part[5] = "\0";
        sprintf(IP_first_part, "%d", z);
        strcat(IP_format, IP_first_part);
        strcat(IP_format, ".");
        sprintf(IP_second_part, "%d", j);
        strcat(IP_format, IP_second_part);
        strcat(IP_format,".0.1:11111");
        j ++;
        if ((i%256 == 0) && (i!=0)) {
            z ++;
            j = 0;
        }
        strcpy(IP_array[i], IP_format);
        i ++;
    }
}

int main(int args, char* argv[]) {

    int row = atoi(argv[2]);
    char** IP_array;
    char** wallet_array;

    IP_array = malloc(sizeof(char *) * row);
    wallet_array = malloc(sizeof(char *) * row);

    for(int i=0; i < row; i++){
        IP_array[i] = malloc(sizeof(char) * 20);
        wallet_array[i] = malloc(sizeof(char) * 40);
    }

    set_IP_and_Address(IP_array, wallet_array, row);

    for(int i=0; i<row; i++) {
        rpc_getnewaddress(IP_array[i], wallet_array[i]);
    }

//    sleep(1200);
    sleep(1500);
    int i=0;
    for(int i=0; i<1; i++){
        int start = rand() % row;
        int end = rand() % row;
        rpc_sendtoaddress(IP_array[start], wallet_array[end]);
        sleep(1);
    }

    free(IP_array);
    free(wallet_array);
}