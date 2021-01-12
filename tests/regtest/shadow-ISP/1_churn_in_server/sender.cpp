//
// Created by ilios on 20. 9. 14..
//
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <memory.h>

int create_connect_socket(char *hostname) {
    int portno = 8333;
    int remote_fd;
    if ( (remote_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
    }

    struct addrinfo* servinfo;
    int n = getaddrinfo(hostname, NULL, NULL, &servinfo);
    if (n != 0) {
        std::cout << "Value of errno: " << errno << "\n";
        fprintf(stderr, "getaddrinfo:%s\n", gai_strerror(n));
        perror("getaddrinfo");
        std::cout << "domain" << hostname << "\n";
    }

    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(portno);
    servaddr.sin_addr.s_addr = ((struct sockaddr_in*) (servinfo->ai_addr))->sin_addr.s_addr;

    if (connect(remote_fd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("connect");
        std::cout << "Unable to connect to " << hostname << "\n";
    }
    std::cout << "connected to " << hostname << "\n";
    return remote_fd;
}

int main(int argc, char *argv[]) {
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char hostname[100] = "11.0.0.10";

    std::cout << "main started for sender" << "\n";

    int remote_fd1 = create_connect_socket("11.0.0.10");
    int remote_fd2 = create_connect_socket("11.0.0.11");

    char hellomsg[100] = "Hello Server 11.0.0.10!!";
    int numbytes = send(remote_fd1, hellomsg, strlen(hellomsg), 0);
    if (numbytes < 0) {
        perror("write error");
        exit(-1);
    }

    char hellomsg2[100] = "Hello Server 11.0.0.11!!";
    numbytes = send(remote_fd2, hellomsg2, strlen(hellomsg2), 0);
    if (numbytes < 0) {
        perror("write error");
        exit(-1);
    }

}
