#include "shadow_interface.h"
#include <stdio.h>

#include <string.h>
#include <stdlib.h>

struct timespec clock_start;


int puts_temp(const char *str) {
    puts("local puts_temp:");
    return puts(str);
}

int shadow_bind(int fd, const struct sockaddr *addr, socklen_t len) {
    printf("local-shadow_bind called\n");
    return 0;
}

int shadow_register_NIC(const struct sockaddr *addr, socklen_t len) {
    printf("shadow_register_NIC call should be intercepted and redirected to shadow");
    exit(-1);
}

int shadow_pipe2(int pipefds[2], int flags) {
    printf("local-shadow_pipe2 called\n");
    return pipe2(pipefds, flags);
}

int shadow_push_eventlog(const char *str) {
    // do nothing
    /* printf("shadow_push_eventlog is ignored for event:%s\n", str); */
    return 0;
}

int shadow_usleep(useconds_t usec) {
    // do nothing
    /* printf("shadow_usleep is ignored\n"); */
    return 0;
}

int shadow_clock_gettime(clockid_t clk_id, struct timespec *tp) {
    // do real clock_gettime
    return clock_gettime(clk_id, tp);
}

int init_shadow_clock_update() {
    return shadow_clock_gettime(CLOCK_MONOTONIC, &clock_start);
}
int next_shadow_clock_update(const char* prefix) {
    struct timespec cur;
    shadow_clock_gettime(CLOCK_MONOTONIC, &cur);
    double nanoseconds = cur.tv_nsec >= clock_start.tv_nsec
                        ? (cur.tv_nsec - clock_start.tv_nsec) + (cur.tv_sec - clock_start.tv_sec) * 1e9
                        : (clock_start.tv_nsec - cur.tv_nsec) + (cur.tv_sec - clock_start.tv_sec - 1) * 1e9;
    shadow_usleep(nanoseconds / 1e3);
    /* printf("%s:%lu\n", prefix, (unsigned long)nanoseconds); */
    /* double milliseconds = cur.tv_nsec >= clock_start.tv_nsec */
    /*                     ? (cur.tv_nsec - clock_start.tv_nsec) / 1e6 + (cur.tv_sec - clock_start.tv_sec) * 1e3 */
    /*                     : (clock_start.tv_nsec - cur.tv_nsec) / 1e6 + (cur.tv_sec - clock_start.tv_sec - 1) * 1e3; */
    /* shadow_usleep(milliseconds * 1000); */
    clock_start = cur;
    return nanoseconds;
}

int shadow_assign_virtual_id() {
    printf("local-shadow_assign_virtual_id called\n");
    return 0;
}


/* HASH TABLE h CONTENT */
// KEY: filename
// Element: LIST x

/* LIST x CONTENT */
// Element 1: unique hash
// Element 2: actual path
// Element 3: reference count
struct shared_file_list {
    char* name;
    struct shared_file_list* prev;
    struct shared_file_list* next;
};
struct shared_file_list_header {
    struct shared_file_list* head;
    struct shared_file_list* tail;
};
struct shared_file_list_header shd_hdr = {NULL, NULL};
struct shared_file_list* find_shared_file(const char * filename) {
    struct shared_file_list* cur = shd_hdr.head;
    while(cur) {
        if(strcmp(filename, cur->name)==0) {
            return cur;
        }
        cur = cur->next;
    }
    return NULL;
}
struct shared_file_list* add_shared_file_list(const char* filename) {
    struct shared_file_list* sfl = (struct shared_file_list*)malloc(sizeof(struct shared_file_list));
    int nameSize = strlen(filename);
    sfl->name = (char*)malloc(sizeof(char)*(nameSize)+1);
    memcpy(sfl->name, filename, nameSize);
    sfl->name[nameSize] = 0;
    sfl->next = NULL;
    if (!shd_hdr.head) {
        sfl->prev = NULL;
        shd_hdr.head = sfl;
    } else {
        sfl->prev = shd_hdr.tail;
        shd_hdr.tail->next = sfl;
    }
    shd_hdr.tail = sfl;
    return sfl;
}
void remove_shared_file_list(struct shared_file_list* sfl) {
    if (sfl->prev) {
        sfl->prev->next = sfl->next;
        if (sfl->next) {
            sfl->next->prev = sfl->prev;
        } else {
            shd_hdr.tail = sfl->prev;
        }
    } else {
        if (sfl->next) {
            sfl->next->prev = NULL;
            shd_hdr.head = sfl->next;
        } else {
            shd_hdr.head = NULL;
            shd_hdr.tail = NULL;
        }
    }
    free(sfl->name);
    free(sfl);
}

void* shadow_claim_shared_entry(void* ptr, size_t sz, int shared_id) {
    printf("shadow_claim_shared_entry in shadow_interface.c\n");
    void* res = malloc(sz);
    memcpy(res, ptr, sz);
    return res;
}