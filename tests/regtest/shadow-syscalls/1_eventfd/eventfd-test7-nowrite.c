//gcc eventfd.c -o eventfd -lpthread
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/eventfd.h>
#include <sys/epoll.h>

int efd = -1;
struct timeval starttv;

void *read_thread(void *dummy)
{
    int ret = 0;
    int ep_fd = -1;

    ep_fd = epoll_create(1024);
    if (ep_fd < 0)
    {
        printf("epoll_create fail: \n"); //replace perror
        goto fail;
    }

    struct epoll_event read_event;

    read_event.events = EPOLLHUP | EPOLLERR | EPOLLIN;
    read_event.data.fd = efd;

    ret = epoll_ctl(ep_fd, EPOLL_CTL_ADD, efd, &read_event);
    if (ret < 0)
    {
        printf("epoll ctl failed:\n"); //replace perror
        goto fail;
    }

    struct epoll_event event;
    ret = epoll_wait(ep_fd, &event, 1, 5000);
    printf("1st epoll_wait ret=%d\n", ret);

    ret = epoll_wait(ep_fd, &event, 1, 5000);
    printf("2nd epoll_wait ret=%d\n", ret);


    if (ret == 1) {
        if (event.events & EPOLLHUP) {
            printf("epoll eventfd has epoll hup.\n");
            goto fail;
        } else if (event.events & EPOLLERR) {
            printf("epoll eventfd has epoll error.\n");
            goto fail;
        } else if (event.events & EPOLLIN) {
            int event_fd = event.data.fd;
            uint64_t count = 0;
            ret = read(event_fd, &count, sizeof(count));
            if (ret < 0) {
                printf("read fail:\n"); //replace perror
                goto fail;
            } else {
                struct timeval tv;

                gettimeofday(&tv, NULL);
                printf("success read from efd, read %d bytes(%lu) at %lds\n",
                       ret, count, tv.tv_sec - starttv.tv_sec);
            }
        }
    } else if (ret == 0) {
        /* time out */
        printf("epoll wait timed out.\n");
    } else {
        printf("epoll wait error:\n"); //replace perror
        goto fail;
    }

    fail:
    if (ep_fd >= 0)
    {
        close(ep_fd);
        ep_fd = -1;
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    gettimeofday(&starttv, NULL);

    pthread_t pid = 0;
    int ret = 0;
    int i = 0;

    //efd = eventfd(0, 0);
    printf("EFD_NONBLOCK:%d\n",EFD_NONBLOCK);
    efd = eventfd(0, EFD_NONBLOCK);
    if (efd < 0)
    {
        printf("eventfd failed.\n"); //replace perror
        goto fail;
    }

    ret = pthread_create(&pid, NULL, read_thread, NULL);
    if (ret < 0)
    {
        printf("pthread create:\n"); //replace perror
        goto fail;
    }


    fail:
    if (0 != pid)
    {
        pthread_join(pid, NULL);
        pid = 0;
    }

    if (efd >= 0)
    {
        close(efd);
        efd = -1;
    }
    /* return ret; */
    return 0; // always return 0.
}

//
// Created by ilios on 20. 6. 30..
//

