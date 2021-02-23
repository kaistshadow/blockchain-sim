#include <stdio.h> /* printf, sprintf */
#include <string.h>
#include <stdlib.h> /* exit */
#include <unistd.h> /* read, write, close */
#include <string.h> /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h> /* struct hostent, gethostbyname */
#include <arpa/inet.h>

void error(const char *msg) {  printf("perror:\n%s\n",msg); exit(1); }
#define h_addr  h_addr_list[0]
int main(void)
{
    /* first what are we going to send and where are we going to send it? */
    char *host = "213.145.108.101"; //127.0.0.1 213.145.108.101
    int portno = 18332;

    char message_fmt[1024];

    strcpy (message_fmt,"GET ");
    strcat(message_fmt, "/rest/chaininfo.json");
    strcat(message_fmt," HTTP/1.0\r\n\r\n");

    struct hostent *server;
    struct sockaddr_in serv_addr;
    int sockfd, bytes, sent, received, total;
    char message[1024],response[4096];

    /* fill in the parameters */
    sprintf(message,message_fmt,host,"18332");
    printf("Request:\n%s\n",message);

    /* create the socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket");

    /* fill in the structure */
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(host); //inet_pton(AF_INET, host, &(serv_addr.sin_addr));
    serv_addr.sin_port = htons(portno);
    /* connect the socket */
    printf("connect the socket start\n");
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting\n");
    printf("connect the socket end\n");
    /* send the request */
    total = strlen(message);
    sent = 0;
    do {
        bytes = write(sockfd,message+sent,total-sent);
        if (bytes < 0)
            error("ERROR writing message to socket");
        if (bytes == 0)
            break;
        sent+=bytes;
    } while (sent < total);
    printf("request send end\n");
    /* receive the response */
    memset(response,0,sizeof(response));
    total = sizeof(response)-1;
    received = 0;
    do {
        bytes = read(sockfd,response+received,total-received);
        if (bytes < 0)
            error("ERROR reading response from socket");
        if (bytes == 0)
            break;
        received+=bytes;
    } while (received < total);
    printf("response receive end\n");
    if (received == total)
        error("ERROR storing complete response from socket");

    /* close the socket */
    close(sockfd);

    /* process response */
    printf("Response:\n%s\n",response);

    return 0;
}