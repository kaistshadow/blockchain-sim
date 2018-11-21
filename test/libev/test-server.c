#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <ev.h>
//#include <glib.h>
#define g_warning printf

//#include <resolv.h>
#include <unistd.h>

#include "array-heap.h"

#define MYPORT 3456    /* the port users will be connecting to */
#define BACKLOG 10     /* how many pending connections queue will hold */

struct sock_ev_serv {
  ev_io io;
  int fd;
  struct sockaddr_in socket;
  array clients;
};

struct sock_ev_client {
  ev_io io;
  int fd;
  int index;
  struct sock_ev_serv* server;
};

int setnonblock(int fd);
static void not_blocked(EV_P_ ev_periodic *w, int revents);

// This callback is called when client data is available
static void client_cb(EV_P_ ev_io *w, int revents) {
  // a client has become readable

  struct sock_ev_client* client = (struct sock_ev_client*) w;

  int n;
  char str[100] = ".\0";

  printf("[r]");
  n = recv(client->fd, str, 100, 0);
  if (n <= 0) {
    if (0 == n) {
      // an orderly disconnect
      puts("orderly disconnect");
      ev_io_stop(EV_A_ &client->io);
      close(client->fd);
    }  else if (EAGAIN == errno) {
      puts("should never get in this state with libev");
    } else {
      perror("recv");
    }
    return;
  } 
  printf("socket client said: %s", str);

  // Assuming that whenever a client is readable, it is also writable ?
  if (send(client->fd, str, n, 0) < 0) {
    perror("send");
  }
}

inline static struct sock_ev_client* client_new(int fd) {
  struct sock_ev_client* client;

  client = (struct sock_ev_client*)realloc(NULL, sizeof(struct sock_ev_client));
  client->fd = fd;
  //client->server = server;
  setnonblock(client->fd);
  ev_io_init(&client->io, client_cb, client->fd, EV_READ);

  return client;
}

// This callback is called when data is readable on the unix socket.
static void server_cb(EV_P_ ev_io *w, int revents) {
  puts("unix stream socket has become readable");

  int client_fd;
  struct sock_ev_client* client;

  // since ev_io is the first member,
  // watcher `w` has the address of the 
  // start of the sock_ev_serv struct
  struct sock_ev_serv* server = (struct sock_ev_serv*) w;

  while (1)
  {
    client_fd = accept(server->fd, NULL, NULL);
    if( client_fd == -1 )
    {
      if( errno != EAGAIN && errno != EWOULDBLOCK )
      {
        g_warning("accept() failed errno=%i (%s)",  errno, strerror(errno));
        exit(EXIT_FAILURE);
      }
      break;
    }
    puts("accepted a client");
    client = client_new(client_fd);
    client->server = server;
    client->index = array_push(&server->clients, client);
    ev_io_start(EV_A_ &client->io);
  }
}

// Simply adds O_NONBLOCK to the file descriptor of choice
int setnonblock(int fd)
{
  int flags;

  flags = fcntl(fd, F_GETFL);
  flags |= O_NONBLOCK;
  return fcntl(fd, F_SETFL, flags);
}

int inet_socket_init(struct sockaddr_in* socket_in) {
  int fd;

  // Setup a unix socket listener.
  fd = socket(AF_INET, SOCK_STREAM, 0);
  if (-1 == fd) {
    perror("echo server socket");
    exit(EXIT_FAILURE);
  }

  // Set it non-blocking
  if (-1 == setnonblock(fd)) {
    perror("echo server socket nonblock");
    exit(EXIT_FAILURE);
  }

  // Set it as INET socket
  socket_in->sin_family = AF_INET;
  socket_in->sin_port = htons(MYPORT);
  socket_in->sin_addr.s_addr = INADDR_ANY; /* auto-fill with my IP */
  bzero(&(socket_in->sin_zero), 8);        /* zero the rest of the struct */

  return fd;
}

int server_init(struct sock_ev_serv* server, int max_queue) {
    server->fd = inet_socket_init(&server->socket);

    array_init(&server->clients, 128);

    if (-1 == bind(server->fd, (struct sockaddr*) &server->socket, sizeof(struct sockaddr)))
    {
      perror("echo server bind");
      exit(EXIT_FAILURE);
    }

    if (-1 == listen(server->fd, max_queue)) {
      perror("listen");
      exit(EXIT_FAILURE);
    }
    return 0;
}

int main(void) {
    int max_queue = 128;
    struct sock_ev_serv server;
    struct ev_periodic every_few_seconds;
    // Create our single-loop for this single-thread application
    EV_P  = ev_default_loop(0);

    // Create unix socket in non-blocking fashion
    server_init(&server, max_queue);

    // To be sure that we aren't actually blocking
    ev_periodic_init(&every_few_seconds, not_blocked, 0, 5, 0);
    ev_periodic_start(EV_A_ &every_few_seconds);

    // Get notified whenever the socket is ready to read
    ev_io_init(&server.io, server_cb, server.fd, EV_READ);
    ev_io_start(EV_A_ &server.io);

    // Run our loop, ostensibly forever
    puts("inet-socket-echo starting...\n");
    ev_loop(EV_A_ 0);

    // This point is only ever reached if the loop is manually exited
    close(server.fd);
    return EXIT_SUCCESS;
}


static void not_blocked(EV_P_ ev_periodic *w, int revents) {
  puts("I'm not blocked");
}
