#include "shadow_interface.h"
#include <stdio.h>

#include <string.h>
#include <stdlib.h>

struct timespec clock_start;

// plugin -> shadow
int puts_temp(const char *str) {
    puts("local puts_temp:");
    return puts(str);
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

char* get_dat_file_path(int fileno) {
    char* test="hello";
    return test;
}

char* get_tmp_file_path() {
    char* test="hello";
    return test;
}

int copy_dat_files(int fileno) {
    printf("copy_dat_files in shadow_interface.c\n");
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


void shadow_shared_try_delete(const char* filename, const char * actual_path) {
    // 1. actual_path에 매칭되는 element를 찾아서 있으면 refcnt--
    // 2. refcnt 가 0이면 파일 제거
    // 3. 자료구조는 지워진 element를 가리키는게 없게끔 할수 있으면 만들고 싶은대로 만들면 됨.
    struct shared_file_list* sfl = find_shared_file(filename);
    if(sfl) {
        remove_shared_file_list(sfl);
    }
}

void shadow_shared_try_create(const char * filename, char* source, long int size, char* actual_path) {
    // 1. filename이랑 매칭되는 파일이 Shadow에서 관리하는 Hash table에 있는지 확인
    //    a. 있으면 해당 table의 element인 list를 가져와서 (이 list에는 unique hash등의 정보가 포함되야함) list의 각 entry의 unique hash와 comparable parameter(ex)source, size)와 compare
    //        i. 매칭되는 애가 있으면 해당 entry의 refcnt++, 해당 entry에 적힌 actual_path를 가져와서 파라미터로 제공받은 actual_path 포인터의포인터로 세팅
    //        ii. 매칭되는 애가 없으면 entry를 주어진 정보를 이용해서 만듦. actual_path도 unique하게 만들어야되고, 해당 entry의 refcnt = 1로 세팅. 만든 entry의 actual_path를 가져와서 파라미터로 제공받은 actual_path로 전달
    //    b. 없으면 list 새로 구성하고 a.ii.과 동일하게 동작
    struct shared_file_list* sfl = find_shared_file(filename);
    if(!sfl) {
        // make actual file
        FILE* fp = fopen(filename, "wb");
        long int nWrite = 0;
        long int totalNWrite = 0;
        while(nWrite = fwrite(&(source[totalNWrite]), sizeof(char), size - totalNWrite, fp)) {
            totalNWrite += nWrite;
            if(totalNWrite >= size)
                break;
        }
        fclose(fp);
        // make actual file tracking list
        sfl = add_shared_file_list(filename);
    }
    memcpy(actual_path, filename, strlen(sfl->name)+1);
}
int compare_dat_files(int fileno) {
    printf("compare_dat_files in shadow_interface.c\n");
    return 0;
}
char* get_actual_path(int fileno){
    printf("get_actual_path in shadow_interface.c\n");
    return NULL;
}

void* shadow_claim_shared_entry(void* ptr, size_t sz, int shared_id) {
    printf("shadow_claim_shared_entry in shadow_interface.c\n");
    void* res = malloc(sz);
    memcpy(res, ptr, sz);
    return res;
}