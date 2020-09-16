#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int counter = 0; 
__thread int * loops;


void assign (int *x)
{
    loops = x;
}

void *worker(void *arg) {
    int x = 10 + counter++;

    assign(&x);
    x = *loops;
    printf("x   : %d\n", x);
    return NULL;
}

int main(int argc, char *argv[]) {
    int i = 5;
    int *arg = &i;

    pthread_t p1, p2;
    assign(arg);
    pthread_create(&p1, NULL, worker, NULL); 
    pthread_create(&p2, NULL, worker, NULL);

    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    printf("args   : %d\n", *loops);
   
    return 0;
}
