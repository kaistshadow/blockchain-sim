#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <iostream>

int main(int argc, char const *argv[]){
    bool am_I_server = (argc > 1);

    if (am_I_server) {
        std::cout << "[Server] i am server\n";

        int socketfd = socket(AF_INET, (SOCK_STREAM), 0);
        if (socketfd == -1) {
            perror("socket");
            exit(1);
        }
        std::cout << "[Server] creating socket done\n";

        struct sockaddr_in my_addr;
        my_addr.sin_family      = AF_INET;
        my_addr.sin_port        = htons(12345);
        my_addr.sin_addr.s_addr = INADDR_ANY;
        bzero(&(my_addr.sin_zero), 8);

        if (bind(socketfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
            perror("bind");
            close(socketfd);
            exit(1);
        }
        std::cout << "[Server] binding socket done\n";

        if (listen(socketfd, 10) == -1) {
            perror("listen");
            close(socketfd);
            exit(1);
        }
        std::cout << "[Server] creating socket done\n";

        struct sockaddr_in client_addr;
        socklen_t sin_size = sizeof(struct sockaddr_in);

        int receiver_fd = accept(socketfd, (struct sockaddr *)&client_addr, &sin_size);
        if (receiver_fd == -1 ) {
            std::cout << "[Server] errno=" << errno << strerror(errno) << "\n";
            return -1;
        }
        std::cout << "[Server] accepting client done\n";

        char string_read[2000];
        int numbytes;

        //numbytes = recv(receiver_fd, string_read, sizeof(int), 0);
        //if (numbytes == 0) {
        //    std::cout << "[Server] disconnect with client done\n";
        //}
        usleep(20000);
        std::cout << "[Server] wait\n";

        numbytes = send(receiver_fd, string_read, 1, 0);
        std::cout << "[Server] send "<<numbytes<<"\n";

    }
    else {
        std::cout << "[Client] i am client\n";

        struct addrinfo* peerinfo;
        int res = getaddrinfo("bleep0", NULL, NULL, &peerinfo);
        if (res == -1) return -1;

        int sfd = socket(AF_INET, (SOCK_STREAM), 0);
        if (sfd == -1) {
            freeaddrinfo(peerinfo);
            return -1;
        }
        std::cout << "[Client] creating socket done\n";

        struct sockaddr_in address;
        memset(&address, 0, sizeof(address));
        address.sin_family      = AF_INET;
        address.sin_port        = htons(12345);
        address.sin_addr.s_addr = ((struct sockaddr_in*)(peerinfo->ai_addr))->sin_addr.s_addr;

        while(1) {
            res = connect(sfd, (struct sockaddr*)&address, sizeof(address));
            if (res == 0) break;
        }
        std::cout << "[Client] connecting to server done\n";

        usleep(3000);
        std::cout << "[Client] wait\n";

        close(sfd);
        std::cout << "[Client] disconnect with server done\n";
        //exit(1);
    }

    return 0;
}
