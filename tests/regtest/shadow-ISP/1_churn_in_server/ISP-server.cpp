//
// Created by ilios on 20. 9. 14..
//

#include <ev++.h>

#include <algorithm>
#include <string_view>
#include <iostream>

#include "shadow_interface.h"
#include <arpa/inet.h>
#include <fcntl.h> /* Added for the nonblocking socket */
#include <memory>

class DataSocketWatcher {
private:
    std::string _shadow_ip;
    int _fd;
    /* event watcher */
    ev::io _watcher;

    /* event io callback */
    void _dataSocketIOCallback(ev::io &w, int revents) {
        std::cout << "data socket IO callback called!" << "\n";

        std::cout << "DoRecv!" << "\n";

        // recv from socket, and append received data into the buffer.
        char string_read[2001];
        int n;
        memset(string_read, 0, 2000);
        n = recv(_fd, string_read, 2000, 0);
        if (n > 0) {
            std::cout << "received data at " << _shadow_ip << ":[" << string_read << "]" << "\n";
        } else if (n == 0) {
            std::cout << "connection closed while recv" << "\n";
        }
    }
public:
    DataSocketWatcher(int fd) {
        _watcher.set<DataSocketWatcher, &DataSocketWatcher::_dataSocketIOCallback> (this);
        _watcher.start(fd, ev::READ);
        _fd = fd;
    }
    DataSocketWatcher(int fd, std::string hostname) {
        _watcher.set<DataSocketWatcher, &DataSocketWatcher::_dataSocketIOCallback> (this);
        _watcher.start(fd, ev::READ);
        _fd = fd;
        _shadow_ip = hostname;
    }
};

class ListenSocketWatcher {
private:
    ev::io listen_watcher;
    std::string _shadow_ip;
    void _listenSocketIOCallback (ev::io &w, int revents) {
        std::cout << "listen socket IO callback called!" << "\n";
        struct 	sockaddr_in 	their_addr; /* connector's address information */
        int sock_fd;
        socklen_t 			sin_size;

        sin_size = sizeof(struct sockaddr_in);
        sock_fd = accept(w.fd, (struct sockaddr *)&their_addr, &sin_size);
        if ( sock_fd != -1 ) {
            std::cout << "server: got connection from " << inet_ntoa(their_addr.sin_addr) << "\n";
            fcntl(sock_fd, F_SETFL, O_NONBLOCK);

            // create event watcher for the DataSocket
            new DataSocketWatcher(sock_fd, _shadow_ip);
        }
        else {
            if( errno != EAGAIN && errno != EWOULDBLOCK ) {
                std::cout << "accept() failed errno=" << errno << strerror(errno) << "\n";
                exit(-1);
            }
        }
    }
public:
    ListenSocketWatcher(char *shadow_ip) {
        _shadow_ip = std::string(shadow_ip);
        int 			new_bindfd;
        struct 	sockaddr_in 	new_addr;    /* my address information */
        if ((new_bindfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            perror("socket");
            exit(1);
        }
        new_addr.sin_family = AF_INET;         /* host byte order */
        new_addr.sin_port = htons(8333);     /* short, network byte order */
////    my_addr.sin_addr.s_addr = INADDR_ANY; /* auto-fill with my IP */
        new_addr.sin_addr.s_addr = inet_addr(shadow_ip);
        bzero(&(new_addr.sin_zero), 8);        /* zero the rest of the struct */

        if (shadow_bind(new_bindfd, (struct sockaddr *)&new_addr, sizeof(struct sockaddr)) == -1) {
            perror("bind");
            exit(1);
        }

        if (listen(new_bindfd, 10) == -1) {
            perror("listen");
            exit(1);
        }
        int flags = fcntl(new_bindfd, F_GETFL, 0);
        fcntl(new_bindfd, F_SETFL, flags | O_NONBLOCK); /* Change the socket into non-blocking state	*/

        listen_watcher.set<ListenSocketWatcher, &ListenSocketWatcher::_listenSocketIOCallback> (this);
        listen_watcher.start(new_bindfd, ev::READ);
    }
};

int main(int argc, char *argv[]) {

    std::cout << "main started for ISP-server" << "\n";

    puts_temp("test shadow_interface\n");

    struct ev_loop *libev_loop = EV_DEFAULT;
    ListenSocketWatcher listenSocketWatcher("11.0.0.10");

    ListenSocketWatcher listenSocketWatcher2("11.0.0.11");

    while (true) {
        std::cout << "before ev_run" << "\n";
        ev_run (libev_loop, EVRUN_ONCE);
    }
}
