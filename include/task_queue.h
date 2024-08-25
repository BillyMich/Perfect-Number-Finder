#ifndef TASK_QUEUE_H
#define TASK_QUEUE_H

#include <gmp.h>
#include <pthread.h>

#define QUEUE_SIZE 1

typedef struct {
    mpz_t numbers[QUEUE_SIZE];
    int front, rear, count;
    pthread_mutex_t mutex;
    pthread_cond_t cond_not_empty;
    pthread_cond_t cond_not_full;
} TaskQueue;

void initTaskQueue();
void destroyTaskQueue();
void enqueue(mpz_t num);
void* worker();

#endif // TASK_QUEUE_H