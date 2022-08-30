#include <omp.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>

void* handler (void*ptr);

sem_t mutex;
int counter;
int op1 = 1;
int op2 = 2;

int main(int argc, char *argv[]) {
    int i[2];
    pthread_t thread_a;
    pthread_t thread_b;

    i[0] = 0;
    i[1] = 1;

    sem_init(&mutex, 0, 1);

    pthread_create(&thread_a, NULL, &handler, &i[0]);
    pthread_create(&thread_b, NULL, &handler, &i[1]);

    pthread_join(thread_a, NULL);
    pthread_join(thread_b, NULL);
    exit(0);
}

void* handler (void*ptr) {
    int x;
    x = *(int*)ptr;
    printf("Thread %d: Waiting to enter critical region...\n", x)
}