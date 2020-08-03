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
    strcat(input, ", 0.01 ]}");
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

/*
 param3 : 0-255 사이
 param4 : _IP_format = "11.0.0.1" ... or ... "11.0.0.n"
 param5 : IP_array Indexing number
 */
void set_IP_and_Wallet(char** IP_array, char** wallet_array, int row, char* _IP_format, int id){

    printf("----------------------------------------------\n");
    printf("%d \n", row);
    printf("%s \n", _IP_format);
    printf("%d \n", id);

    int j = 1;
    for(int i=id; i<row + id; i++){
        IP_array[i] = malloc(sizeof(char) * 20);
        wallet_array[i] = malloc(sizeof(char) * 40);

        char IP_format[20] = "\0";
        char IP_4th_class[4] = "\0";

        strcpy(IP_format, _IP_format);
        sprintf(IP_4th_class, "%d", j);
        strcat(IP_format, IP_4th_class);
        strcat(IP_format,":11111");
        j ++;
        strcpy(IP_array[i], IP_format);
    }

}

int main(int args, char* argv[]) {

    int row = atoi(argv[2]);
    char** IP_array = "\0";
    char** wallet_array = "\0";

    IP_array = malloc(sizeof(char *) * row);
    wallet_array = malloc(sizeof(char *) * row);
    int found = 0; // 예외처리 변수

    // node < 256
    if (row < 256) {
        char IP_format[20] = "11.0.0.";
        set_IP_and_Wallet(IP_array, wallet_array, row, IP_format,0);
    } else {
        char IP_format[20] = "11.0.0.";
        set_IP_and_Wallet(IP_array, wallet_array, 255, IP_format,0);

        int IPCount = row / 255;
        for(int i=0; i < IPCount; i++){
            char IP_format[20] = "11.0.";
            char IP_3th_class[4] = "\0";
            sprintf(IP_3th_class, "%d", i+1);
            strcat(IP_format, IP_3th_class);
            strcat(IP_format, ".");
            if(i+1 == IPCount){
                int rest_Nnode = (row % 255);
                set_IP_and_Wallet(IP_array, wallet_array, rest_Nnode, IP_format, IPCount*255);
            } else {
                set_IP_and_Wallet(IP_array, wallet_array, 255, IP_format, (i+1)*255);
            }
        }
    }
//     전체 노드에게 TX 브로드 캐스팅
//     TODO : 노드 수가 늘어날 수록 랜덤으로 특정 노드에게만 브로드캐스팅 하게 하
     for(int i=0; i<row; i++){
         strcpy(wallet_array[i], "\0");
         rpc_getnewaddress(IP_array[i], wallet_array[i]);
     }
    for(int i =0; i < row; i++ ){
        printf("%s \n", IP_array[i]);
        printf("%s \n", wallet_array[i]);
        printf("--------------------------\n");
    }
    free(IP_array);
    free(wallet_array);

    srand(time(NULL));
    while(1) {
        int index = (rand() % row) +1;
        int _index = (rand() % row) +1;
        rpc_sendtoaddress(IP_array[index], wallet_array[_index]);
        sleep(0.5);
    }

}