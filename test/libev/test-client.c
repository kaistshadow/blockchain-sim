#include <stdlib.h>
#include <stdio.h>
#include <ev.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <sys/fcntl.h> // fcntl
#include <unistd.h> // close

#define MYPORT 3456    /* the port users will be connecting to */
#define BACKLOG 10     /* how many pending connections queue will hold */

// TODO
// disconnect and reconnect on each newline

// Nasty globals for now
// feel free to fork this example and clean it up
EV_P;
ev_io stdin_watcher;
ev_io remote_w;
ev_io send_w;
int remote_fd;
char* line = NULL;
size_t len = 0;

/* static void send_cb (EV_P_ ev_io *w, int revents) */
/* { */
/*   if (revents & EV_READ) */
/*   { */
/*     int n; */
/*     char str[100] = ".\0"; */

/*     printf("[r,remote]"); */
/*     n = recv(remote_fd, str, 100, 0); */
/*     if (n <= 0) { */
/*       if (0 == n) { */
/*         // an orderly disconnect */
/*         puts("orderly disconnect"); */
/*         ev_io_stop(EV_A_ &send_w); */
/*         close(remote_fd); */
/*       }  else if (EAGAIN == errno) { */
/*         puts("should never get in this state with libev"); */
/*       } else { */
/*         perror("recv"); */
/*       } */
/*       return; */
/*     } */
/*     printf("socket client said: %s", str); */

/*   } */
/* } */

/* static void remote_cb (EV_P_ ev_io *w, int revents) */
/* { */
/*   puts ("connected, now sending msg"); */

/*   char msg[100] = "hello world!"; */
/*   if (-1 == send(remote_fd, msg, strlen(msg), 0)) { */
/*       perror("echo send"); */
/*       exit(EXIT_FAILURE); */
/*   } */
/*   // Once we're connected, that's the end of that */
/*   ev_io_stop(EV_A_ &remote_w); */
/* } */


// Simply adds O_NONBLOCK to the file descriptor of choice
int setnonblock(int fd)
{
  int flags;

  flags = fcntl(fd, F_GETFL);
  flags |= O_NONBLOCK;
  return fcntl(fd, F_SETFL, flags);
}

static void connection_new(EV_P_ char* servname) {
  struct sockaddr_in remote;

  if (-1 == (remote_fd = socket(AF_INET, SOCK_STREAM, 0))) {
      perror("socket");
      exit(1);
  }

  // Set it non-blocking
  /* if (-1 == setnonblock(remote_fd)) { */
  /*   perror("echo client socket nonblock"); */
  /*   exit(EXIT_FAILURE); */
  /* } */

  // this should be initialized before the connect() so
  // that no packets are dropped when initially sent?
  // http://cr.yp.to/docs/connect.html

  remote.sin_family = AF_INET;
  remote.sin_port = htons(MYPORT);

  struct addrinfo* servinfo;
  if (getaddrinfo(servname, NULL, NULL, &servinfo) != 0) {
      perror("echo client getaddrinfo");
      exit(EXIT_FAILURE);
  }
  remote.sin_addr.s_addr = ((struct sockaddr_in*) (servinfo->ai_addr))->sin_addr.s_addr;


  if (-1 == connect(remote_fd, (struct sockaddr *)&remote, sizeof(struct sockaddr_in))) {
      perror("connect");
      //exit(1);
  }
  printf("connected!\n");

  char msg[100] = "hello world!";
  if (-1 == send(remote_fd, msg, strlen(msg), 0)) {
      perror("echo send");
      exit(EXIT_FAILURE);
  }
}

int main (int argc, char *argv[])
{
    if (argc != 2) {
        printf("USAGE:./test-client localhost\n");
        exit(-1);
    }
  loop = EV_DEFAULT;

  connection_new(EV_A_ argv[1]);

  // now wait for events to arrive
  ev_loop(EV_A_ 0);

  // unloop was called, so exit
  return 0;
}
