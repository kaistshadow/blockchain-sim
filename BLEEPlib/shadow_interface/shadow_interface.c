#include "shadow_interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void hj_interposer_test() {
    printf(" hj !!! - hj_interposer_test in shadow_interface.c\n");
    return;
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

void shadow_shared_try_delete(const char* filename, const char * actual_path) {
    // 1. actual_path에 매칭되는 element를 찾아서 있으면 refcnt--
    // 2. refcnt 가 0이면 파일 제거
    // 3. 자료구조는 지워진 element를 가리키는게 없게끔 할수 있으면 만들고 싶은대로 만들면 됨.
}

void shadow_shared_try_create(const char * filename, char* source, long int size, char** actual_path) {
    // 1. filename이랑 매칭되는 파일이 Shadow에서 관리하는 Hash table에 있는지 확인
    //    a. 있으면 해당 table의 element인 list를 가져와서 (이 list에는 unique hash등의 정보가 포함되야함) list의 각 entry의 unique hash와 comparable parameter(ex)source, size)와 compare
    //        i. 매칭되는 애가 있으면 해당 entry의 refcnt++, 해당 entry에 적힌 actual_path를 가져와서 파라미터로 제공받은 actual_path 포인터의포인터로 세팅
    //        ii. 매칭되는 애가 없으면 entry를 주어진 정보를 이용해서 만듦. actual_path도 unique하게 만들어야되고, 해당 entry의 refcnt = 1로 세팅. 만든 entry의 actual_path를 가져와서 파라미터로 제공받은 actual_path로 전달
    //    b. 없으면 list 새로 구성하고 a.ii.과 동일하게 동작
}
int compare_dat_files(int fileno) {
    printf("compare_dat_files in shadow_interface.c\n");
    return 0;
}

//hj add for storage hashtable
/*
int makehash(int value, int hashSize){
    return value%hashSize;
}
*/

HashTable *createHashTable() {
    HashTable *tbl = (HashTable*)malloc(sizeof(HashTable));
    tbl->ents = (HashTblEntry*)malloc(sizeof(HashTblEntry)*10);
    // initialize hash table entries
    for(int i=0; i<10; i++) {
        tbl->ents[i].listcnt = 0;
        tbl->ents[i].list = NULL;
    }
    return tbl;
};


// AddHashData : [key]에 data 추가 -
void AddHashData(HashTable *hashTable, int key, char* actual_path, char* lastBlockHash){

    // list entry 생성
    Hashlist* elem = (Hashlist*)malloc(sizeof(Hashlist));
    elem->fileno=fileno;
    elem->actual_path=actual_path;
    elem->lasBlockHashMerkleRoot=lastBlockHash;
    elem->refCnt=0;

    // put elem to list header
    Hashlist* cursor = hashTable->ents[key].list;
    hashTable->ents[key].list = elem;
    elem->n = cursor;
    elem->prev = NULL;
    if (cursor)
        cursor->prev = elem;

    hashTable->ents[key].listcnt++;
}

char* getLastBlockHash(HashTable *hashTable, int key){
    char* res;
//    =malloc(sizeof(hashTable->ents[key].list->lasBlockHashMerkleRoot));
    res=hashTable->ents[key].list->lasBlockHashMerkleRoot;
    return res;
}

void DeleteHashData(HashTable *hashTable, int key, char* actual_path){

    if(hashTable->ents[key].list==NULL) {
        return;
    }

    Hashlist* delNode = NULL;
    if(hashTable->ents[key].list->actual_path==actual_path){
        delNode=&hashTable->ents[key];
        hashTable->ents[key].list=hashTable->ents->list->n;
    }
    else {
        Hashlist *node = hashTable->ents[key].list;
        Hashlist *next = node->n;

        while (next) {
            if (strcmp(next->actual_path ,actual_path) == 0) {
                node->n = next->n;
                delNode = next;
                break;
            }
            node = next;
            next = node->n;
        }
    }
    free(delNode);
}
char * makeActualPath(int fileno, char* nodeid) {
    char *path;
    path=malloc(30);
    sprintf(path,"cp_data/%s/dat_%d.dat",nodeid,fileno);
    return path;
}

Hashlist* FindHashData(HashTable* hashTable,int key) {
    if (hashTable->ents[key].list == NULL)
        return NULL;

    if (hashTable->ents[key].list->fileno == key) {
        return hashTable->ents[key].list;
    }
    else {
        Hashlist * node = hashTable->ents[key].list;
        while (node->n) {
            if (node->n->fileno == key) {
                return node->n;
            }
            node = node->n;
        }
    }
    return  NULL;
}

void printHashTable(HashTable *hashTable,int key){
    Hashlist * node = hashTable->ents[key].list;
    printf("print hash table : %s \n",node->actual_path);
    while (node->n) {
        node = node->n;
        printf("print hash table : %s \n",node->actual_path);

    }
}

void teshashmap(){
    HashTable *tbl = createHashTable(sizeof(HashTable));

    AddHashData(tbl,0,makeActualPath(10,"node1"),makeActualPath(20,"node10"));
    AddHashData(tbl,0,makeActualPath(11,"node2"),makeActualPath(21,"node11"));
    AddHashData(tbl,0,makeActualPath(12,"node3"),makeActualPath(22,"node12"));
    printHashTable(tbl,0);

    DeleteHashData(tbl,0,"cp_data/node2/dat_11.dat");
    printHashTable(tbl,0);
    printf("print getlastblockhash = %s \n",getLastBlockHash(tbl,0));

}
