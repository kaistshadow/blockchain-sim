#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/epoll.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <time.h>
#include "sha256.h"
#include <arpa/inet.h>

/* #define BITCOIN_PORT 18444 */
#define BITCOIN_PORT 8333

static const char* USAGE = "USAGE: injector <server_hostname>\n";

#define LOG_ERROR 1
#define LOG_WARNING 2
#define LOG_INFO 3
#define LOG_DEBUG 4

static void _log(int level, const char* functionName, const char* format, ...) {
    char* levelString = "unknown";
    if(level == LOG_ERROR) {
        levelString = "error";
    } else if(level == LOG_WARNING) {
        levelString = "warning";
    } else if(level == LOG_INFO) {
        levelString = "info";
    } else if(level == LOG_DEBUG) {
        levelString = "debug";
    }

    printf("%li [%s] [%s] ", (long int) time(NULL), levelString, functionName);

    va_list variableArguments;
    va_start(variableArguments, format);
    vprintf(format, variableArguments);
    va_end(variableArguments);

    printf("%s", "\n");
}

static int
connect_to_server (char *serverHostname)
{
    int s, sfd;

    struct addrinfo* serverInfo;
    s = getaddrinfo(serverHostname, NULL, NULL, &serverInfo);
    if (s != 0) {
        _log(LOG_ERROR, __FUNCTION__, "unable to start client: error in getaddrinfo");
        return -1;
    }

    in_addr_t serverIP = ((struct sockaddr_in*) (serverInfo->ai_addr))->sin_addr.s_addr;
    freeaddrinfo(serverInfo);

    /* create the client socket and get a socket descriptor */
    sfd = socket(AF_INET, (SOCK_STREAM), IPPROTO_TCP);
    if (sfd == -1) {
        _log(LOG_ERROR, __FUNCTION__, "unable to start client: error in socket");
        return -1;
    }

    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = serverIP;
    serverAddress.sin_port = htons(BITCOIN_PORT);

    /* connect to server. since we are non-blocking, we expect this to return EINPROGRESS */
    int res = connect(sfd, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
    if (res == -1 && errno != EINPROGRESS) {
        _log(LOG_ERROR, __FUNCTION__, "unable to start client: error in connect");
        return -1;
    }
    else if (res == -1) {
        _log(LOG_ERROR, __FUNCTION__, "unable to start client: error in connect");
        return -1;
    }
    else {
        _log(LOG_INFO, __FUNCTION__, "connect established with socketfd %d", sfd);
    }
    return sfd;
}

static void make_message(unsigned char *msg, unsigned int magic, const char *command, const char *payload, unsigned payload_size) {
    int message_size = 4 + 12 + 4 + 4 + payload_size;

    /* calculate checksum of the message */
    BYTE buf[SHA256_BLOCK_SIZE];
    BYTE checksum[SHA256_BLOCK_SIZE];
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, payload, payload_size);
    sha256_final(&ctx, buf);

    sha256_init(&ctx);
    sha256_update(&ctx, buf, SHA256_BLOCK_SIZE);
    sha256_final(&ctx, checksum);
    

    /* create message of bitcoin protocol */
    memcpy(msg, &magic, sizeof(int));
    memcpy(msg+4, command, 12);
    memcpy(msg+4+12, &payload_size, sizeof(unsigned));
    memcpy(msg+4+12+4, checksum, 4);
    memcpy(msg+4+12+4+4, payload, payload_size);

    /* print created message */
    printf("msg=");
    for (int i = 0; i < message_size; i++) printf("%02x", msg[i]);
    printf("\n");

    return;
}

unsigned long long llrand() {
    unsigned long long r = 0;

    for (int i = 0; i < 5; ++i) {
        r = (r << 15) | (rand() & 0x7FFF);
    }

    return r & 0xFFFFFFFFFFFFFFFFULL;
}

int main(int argc, char *argv[]) {
    _log(LOG_INFO, __FUNCTION__, "Starting program");

    /* create network socket to bitcoin peer */
    int sfd = connect_to_server(argv[1]);

    unsigned int magic = 0xd9b4bef9; // mainnet
    /* unsigned int magic = 0xdab5bffa; // testnet */
    char command[12];

    /* create version message payload */
    int version = 70015;
    uint64_t services = 0;
    int64_t timestamp = (int64_t)time(NULL);
    uint32_t ipaddr = inet_addr("127.0.0.1");
    uint16_t port = htons(BITCOIN_PORT);
    uint64_t nonce = llrand();
    BYTE user_agent_bytes = 0;
    int32_t start_height = 0;
    BYTE relay = 1;

    int ver_payload_size = 4+8+8+26+26+8+1+4+1;
    BYTE ver_payload[ver_payload_size];
    memcpy(ver_payload, &version, sizeof(int));
    memcpy(ver_payload+4, &services, sizeof(uint64_t));
    memcpy(ver_payload+4+8, &timestamp, sizeof(int64_t));
    memcpy(ver_payload+4+8+8, &services, sizeof(uint64_t));
    const char prefix_ipv4[12] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff};
    memcpy(ver_payload+4+8+8+8, prefix_ipv4 ,12);
    memcpy(ver_payload+4+8+8+8+12, &ipaddr, sizeof(uint32_t));
    memcpy(ver_payload+4+8+8+8+12+4, &port, sizeof(uint16_t));
    memcpy(ver_payload+4+8+8+26, &services, sizeof(uint64_t));
    memcpy(ver_payload+4+8+8+26+8, prefix_ipv4 ,12);
    memcpy(ver_payload+4+8+8+26+8+12, &ipaddr, sizeof(uint32_t));
    memcpy(ver_payload+4+8+8+26+8+12+4, &port, sizeof(uint16_t));
    memcpy(ver_payload+4+8+8+26+26, &nonce, sizeof(uint64_t));
    memcpy(ver_payload+4+8+8+26+26+8, &user_agent_bytes, sizeof(BYTE));
    memcpy(ver_payload+4+8+8+26+26+8+1, &start_height, sizeof(int32_t));
    memcpy(ver_payload+4+8+8+26+26+8+1+4, &relay, sizeof(BYTE));
    printf("ver_payload=");
    for (int i = 0; i < ver_payload_size; i++) printf("%02x", ver_payload[i]);
    printf("\n");

    /* create version message */
    bzero(command, 12);
    sprintf(command, "version");
    int msg_size = 4 + 12 + 4 + 4 + ver_payload_size;
    unsigned char *version_msg = (unsigned char *)malloc(msg_size);
    make_message(version_msg, magic, command, ver_payload, ver_payload_size);
    printf("ver_message=");
    for (int i = 0; i < msg_size; i++) printf("%02x", version_msg[i]);
    printf("\n");

    /* send version message to bitcoin peer */
    int numBytes = send (sfd, version_msg, msg_size, 0);
    if ( numBytes != msg_size) {
        _log(LOG_ERROR, __FUNCTION__, "Error while write to socket");
        return -1;
    }
    else {
        _log(LOG_INFO, __FUNCTION__, "Sent version message to peer");
        free(version_msg);
    }

    /* receive version reply */
    char message[1000];
    bzero(message, 1000);
    int n = 0;
    int fail_count = 0;
    n = recv(sfd, message, sizeof(message)-1, 0);
    if (n > 0) {
        printf("numBytes received=%d, message=[%s]\n", n, message);
    }
    else if (n == -1) {
        _log(LOG_ERROR, __FUNCTION__, "Error while read socket");
    }
    /* while (1) { */
    /*     while ( (n = recv(sfd, message, sizeof(message)-1, 0)) > 0 ){ */
    /*         printf("numBytes received=%d, message=[%s]\n", n, message); */
    /*     } */
    /*     if (n == -1) { */
    /*         sleep(1); */
    /*         fail_count++; */
    /*         if (fail_count == 3) */
    /*             break; */
    /*     } */
    /* } */

    /* create verack message */
    bzero(command, 12);
    sprintf(command, "verack");
    char *empty_payload = "";
    msg_size = 4 + 12 + 4 + 4 + strlen(empty_payload);
    char *verack_msg = (char *)malloc(msg_size);
    make_message(verack_msg, magic, command, empty_payload, strlen(empty_payload));
    
    /* send verack message to bitcoin peer */
    numBytes = send (sfd, verack_msg, msg_size, 0);
    if ( numBytes != msg_size) {
        _log(LOG_ERROR, __FUNCTION__, "Error while write to socket");
        return -1;
    }
    else {
        _log(LOG_INFO, __FUNCTION__, "Sent verack to peer");
        free(verack_msg);
    }

    /* receive verack reply */
    bzero(message, 1000);
    n = 0;
    n = recv(sfd, message, sizeof(message)-1, 0);
    if (n > 0) {
        printf("numBytes received=%d, message=[%s]\n", n, message);
    }
    else if (n == -1) {
        _log(LOG_ERROR, __FUNCTION__, "Error while read socket");
    }

    /* create generate message */
    bzero(command, 12);
    sprintf(command, "generate");
    msg_size = 4 + 12 + 4 + 4 + strlen(empty_payload);
    char *generate_msg = (char *)malloc(msg_size);
    make_message(generate_msg, magic, command, empty_payload, strlen(empty_payload));

    /* send generate message to bitcoin peer */
    numBytes = send (sfd, generate_msg, msg_size, 0);
    if ( numBytes != msg_size) {
        _log(LOG_ERROR, __FUNCTION__, "Error while write to socket");
        return -1;
    }
    else {
        _log(LOG_INFO, __FUNCTION__, "Sent generate message to peer");
        free(generate_msg);
    }

    bzero(message, 1000);
    n = 0;
    n = recv(sfd, message, sizeof(message)-1, 0);
    if (n > 0) {
        printf("numBytes received=%d, message=[%s]\n", n, message);
    }
    else if (n == -1) {
        _log(LOG_ERROR, __FUNCTION__, "Error while read socket");
    }

    return 0;
}

