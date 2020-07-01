//gcc eventfd.c -o eventfd -lpthread
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/timerfd.h>
#include <sys/epoll.h>


int tfd = -1;
struct timeval starttv;

void *read_thread(void *dummy)
{
    int ret = 0;
    uint64_t count = 0;
    int ep_fd = -1;
    struct epoll_event events[10];

    if (tfd < 0)
    {
        printf("tfd not inited.\n");
        goto fail;
    }

    ep_fd = epoll_create(1024);
    if (ep_fd < 0)
    {
        printf("epoll_create fail: \n"); //replace perror
        goto fail;
    }


    struct epoll_event read_event;

    read_event.events = EPOLLHUP | EPOLLERR | EPOLLIN;
    read_event.data.fd = tfd;

    ret = epoll_ctl(ep_fd, EPOLL_CTL_ADD, tfd, &read_event);
    if (ret < 0)
    {
        printf("epoll ctl failed:\n"); //replace perror
        goto fail;
    }


    while (1)
    {
        ret = epoll_wait(ep_fd, &events[0], 10, 5000);
        printf("epoll_wait ret=%d\n", ret);
        if (ret > 0)
        {
            int i = 0;
            for (; i < ret; i++)
            {
                if (events[i].events & EPOLLHUP)
                {
                    printf("epoll timerfd has epoll hup.\n");
                    goto fail;
                }
                else if (events[i].events & EPOLLERR)
                {
                    printf("epoll timerfd has epoll error.\n");
                    goto fail;
                }
                else if (events[i].events & EPOLLIN)
                {
                    int event_fd = events[i].data.fd;
                    ret = read(event_fd, &count, sizeof(count));
                    if (ret < 0)
                    {
                        printf("read fail:\n"); //replace perror
                        goto fail;
                    }
                    else
                    {
                        struct timeval tv;

                        gettimeofday(&tv, NULL);
                        printf("success read from tfd, read %d bytes(%lu) at %lds\n",
                               ret, count, tv.tv_sec - starttv.tv_sec);
                    }
                }
            }
        }
        else if (ret == 0)
        {
            /* time out */
            printf("epoll wait timed out.\n");
            break;
        }
        else
        {
            printf("epoll wait error:\n"); //replace perror
            goto fail;
        }
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
    uint64_t count = 0;
    int ret = 0;
    int i = 0;

    tfd = timerfd_create(CLOCK_MONOTONIC, 0);
    if (tfd < 0) {
        printf("timerfd_create failed.\n"); //replace perror
        goto fail;
    }
    printf("created timerfd %d\n", tfd);

    struct itimerspec ts;
    int msec = 10; // timer fires after 10msec
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;
    ts.it_value.tv_sec = msec / 1000;
    ts.it_value.tv_nsec = (msec % 1000) * 1000000;

    if (timerfd_settime(tfd, 0, &ts, NULL) < 0) {
        printf("timerfd_settime failed\n"); //replace perror
        close(tfd);
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

    if (tfd >= 0)
    {
        close(tfd);
        tfd = -1;
    }
    /* return ret; */
    return 0; // always return 0.
}


//
// Created by ilios on 20. 6. 30..
//

