//
// Created by ilios on 20. 9. 14..
//

#include <ev++.h>

#include <algorithm>
#include <iostream>

#include "shadow_interface.h"
#include <arpa/inet.h>
#include <fcntl.h> /* Added for the nonblocking socket */
#include <memory>
#include <netinet/in.h>
#include <netdb.h>


int create_connect_socket(char *hostname, char *shadow_ip) {
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

    // bind to shadow network interface
    struct 	sockaddr_in 	shadow_addr;    /* my address information */
    shadow_addr.sin_family = AF_INET;         /* host byte order */
    shadow_addr.sin_port = htons(8333);     /* short, network byte order */
    shadow_addr.sin_addr.s_addr = inet_addr(shadow_ip);
    bzero(&(shadow_addr.sin_zero), 8);        /* zero the rest of the struct */

    if (shadow_bind(remote_fd, (struct sockaddr *)&shadow_addr, sizeof(struct sockaddr)) == -1) {
        perror("bind");
        exit(1);
    }

    // connect to victim address
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

    std::cout << "main started for ISP-attacker" << "\n";


    int remote_fd1 = create_connect_socket("11.0.0.2", "11.0.0.10");
    int remote_fd2 = create_connect_socket("11.0.0.2", "11.0.0.11");

    char hellomsg[100] = "Hello victim! I'm 11.0.0.10!!";
    int numbytes = send(remote_fd1, hellomsg, strlen(hellomsg), 0);
    if (numbytes < 0) {
        perror("write error");
        exit(-1);
    }

    char victim_message[2000];
    int n = recv(remote_fd1, victim_message , 2000 , 0);
    if (n > 0) {
        std::cout << "received data from victim:[" << victim_message << "]" << "\n";
    } else if (n == 0) {
        std::cout << "connection closed while recv" << "\n";
    }

    char hellomsg2[100] = "Hello victim! I'm 11.0.0.11!!";
    numbytes = send(remote_fd2, hellomsg2, strlen(hellomsg2), 0);
    if (numbytes < 0) {
        perror("write error");
        exit(-1);
    }

    bzero(victim_message, 2000);
    n = recv(remote_fd2, victim_message , 2000 , 0);
    if (n > 0) {
        std::cout << "received data from victim:[" << victim_message << "]" << "\n";
    } else if (n == 0) {
        std::cout << "connection closed while recv" << "\n";
    }
}
