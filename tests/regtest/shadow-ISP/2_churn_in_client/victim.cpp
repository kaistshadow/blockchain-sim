//
// Created by ilios on 20. 9. 16..
// from https://dzone.com/articles/parallel-tcpip-socket-server-with-multi-threading
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


struct arg_struct {
    int socket;
    char *remote_ip;
};

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
void * socketThread(void *arg)
{
    struct arg_struct args = *((struct arg_struct *)arg);
    int newSocket = args.socket;
    char *remote_ip = args.remote_ip;

    char client_message[2000];
    int n = recv(newSocket , client_message , 2000 , 0);

    if (n > 0) {
        std::cout << "received data from " << remote_ip << ":[" << client_message << "]" << "\n";
    } else if (n == 0) {
        std::cout << "connection closed while recv" << "\n";
    }

    // Send message to the client socket
    char buffer[2020];
    bzero(buffer, 2020);
    strcpy(buffer,"Hello Client : ");
    strcat(buffer,client_message);
    sleep(1);
    send(newSocket,buffer,strlen(buffer),0);
    printf("Exit socketThread \n");

    close(newSocket);
    pthread_exit(NULL);
}



int main(int argc, char *argv[]) {
    int serverSocket, newSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_in theirAddr;
    socklen_t addr_size;

    //Create the socket.
    serverSocket = socket(PF_INET, SOCK_STREAM, 0);

    // Configure settings of the server address struct
    // Address family = Internet
    serverAddr.sin_family = AF_INET;

    //Set port number, using htons function to use proper byte order
    serverAddr.sin_port = htons(8333);

//    //Set IP address to localhost
//    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_addr.s_addr = INADDR_ANY; /* auto-fill with my IP */

    //Set all bits of the padding field to 0
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    //Bind the address struct to the socket
    bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

    //Listen on the socket, with 40 max connection requests queued
    if(listen(serverSocket,50)==0)
        printf("Listening\n");
    else
        printf("Error\n");

    pthread_t tid[60];

    int i = 0;
    while(1)
    {
        //Accept call creates a new socket for the incoming connection
        addr_size = sizeof theirAddr;
        newSocket = accept(serverSocket, (struct sockaddr *) &theirAddr, &addr_size);
        //for each client request creates a thread and assign the client request to it to process

        char *remote_ip= (char*) malloc(16);
        strncpy(remote_ip,inet_ntoa(theirAddr.sin_addr), 16 );
        std::cout << "victim: got connection from " << remote_ip << "\n";

        //so the main thread can entertain next request
        struct arg_struct args;
        args.socket = newSocket;
        args.remote_ip = remote_ip;

        if( pthread_create(&tid[i++], NULL, socketThread, &args) != 0 )
            printf("Failed to create thread\n");

        if( i >= 50)
        {
            i = 0;
            while(i < 50)
            {
                pthread_join(tid[i++],NULL);
            }
            i = 0;
        }
    }

    return 0;
}
