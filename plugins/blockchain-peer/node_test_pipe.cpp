#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <ev++.h>
#include <fcntl.h>
#include <string.h>
#include "shadow_interface/shadow_interface.h"

int fd[2];//File descriptor for creating a pipe
int shadow_fd[2]; // descriptor for creating a shadow_pipe

//This function continously reads fd[0] for any input data byte
//If available, prints

// void *reader(void*)
// {
//     while(1){
//         char    ch;
//         int     result;

//         result = read (fd[0],&ch,1);
//         if (result != 1) {
//             perror("read");
//             exit(3);
//         }    printf ("Reader: %c\n", ch);  }
// }

static void io_cb (ev::io &w, int revents) {
    char string_read[2000];
    memset(string_read, 0, 2000);
    int n;
    n = read(fd[0], string_read, 2000);
    if (n == 2000) {
        printf("read : n=%d, string_read=%s\n", n, string_read);
        printf("read should be continued!\n");
    }
    else if (n >= 0 && n < 2000) {
        printf("read done : n=%d, string_read=%s\n", n, string_read);
        w.stop();
    }
    else {
        perror("read error");
        exit(-1);
    }
}

//This function continously writes Capital Alphabet into fd[1]
//Waits if no more space is available

void *writer_ABC(void*)
{
    int     result;
    char    ch='A';

    while(1){
        result = write (fd[1], &ch,1);
        if (result != 1){
            perror ("write");
            exit (2);
        }

        printf ("Writer_ABC: %c\n", ch);
        if(ch == 'Z')
            break;
            // ch = 'A'-1;

        ch++;
    }
}

//This function continously writes small Alphabet into fd[1]
//Waits if no more space is available

void *writer_abc(void*)
{
    int     result;  char    ch='a';

    while(1){
        result = write (fd[1], &ch,1);
        if (result != 1){
            perror ("write");
            exit (2);
        }

        printf ("Writer_abc: %c\n", ch);
        if(ch == 'z') 
            break;
            // ch = 'a'-1;

        ch++;
    }
}

int main()
{
    pthread_t       tid1,tid2,tid3;
    int             result;

    result = pipe2 (fd, 0);
    if (result < 0){
        perror("pipe ");
        exit(1);
    }

    // pthread_create(&tid1,NULL,reader,NULL);
    pthread_create(&tid2,NULL,writer_ABC,NULL);
    pthread_create(&tid3,NULL,writer_abc,NULL);

    // pthread_join(tid1,NULL);
    pthread_join(tid2,NULL);
    pthread_join(tid3,NULL);


    fcntl(fd[0], F_SETFL, O_NONBLOCK); /* Change the fd into non-blocking state	*/

    ev::default_loop loop;
    ev::io iow;
    iow.set <io_cb> ();
    iow.start(fd[0], ev::READ);



    // test for shadow-pipe
    result = shadow_pipe2 (shadow_fd, 0);
    if (result < 0){
        perror("pipe ");
        exit(1);
    }
    ev::io shadow_iow;
    shadow_iow.set <io_cb> ();
    shadow_iow.start(shadow_fd[0], ev::READ);
    
    loop.run(0);
}
