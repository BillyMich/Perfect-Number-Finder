#include <pthread.h>
#include <gmp.h>
#include <stdio.h>
#include "../include/task_queue.h"
#include "../include/perfect_number.h"
#include "../include/ui.h"

TaskQueue queue;
extern int perfect_number_found;
extern time_t program_start;

void initTaskQueue() {
    queue.front = 0;
    queue.rear = 0;
    queue.count = 0;
    pthread_mutex_init(&queue.mutex, NULL);
    pthread_cond_init(&queue.cond_not_empty, NULL);
    pthread_cond_init(&queue.cond_not_full, NULL);
}

void destroyTaskQueue() {
    pthread_mutex_destroy(&queue.mutex);
    pthread_cond_destroy(&queue.cond_not_empty);
    pthread_cond_destroy(&queue.cond_not_full);
}

void enqueue(mpz_t num) {
    pthread_mutex_lock(&queue.mutex);
    while (queue.count == QUEUE_SIZE) {
        pthread_cond_wait(&queue.cond_not_full, &queue.mutex);
    }
    mpz_init_set(queue.numbers[queue.rear], num);
    queue.rear = (queue.rear + 1) % QUEUE_SIZE;
    queue.count++;
    pthread_cond_signal(&queue.cond_not_empty);
    pthread_mutex_unlock(&queue.mutex);
}

void* worker() {
    char log[256];
    snprintf(log, sizeof(log), "Worker thread started.");
    updateLogWindow(log);

    while (1) {
        pthread_mutex_lock(&queue.mutex);
        while (queue.count == 0) {
            pthread_cond_signal(&queue.cond_not_full);
            pthread_cond_wait(&queue.cond_not_empty, &queue.mutex);
        }
        mpz_t num;
        mpz_init_set(num, queue.numbers[queue.front]);
        queue.front = (queue.front + 1) % QUEUE_SIZE;
        queue.count--;
        pthread_mutex_unlock(&queue.mutex);

        if (isPerfectNumber(num)) {
            gmp_snprintf(log, sizeof(log), "%Zd is a perfect number. Time taken: %.2f  seconds", num, difftime(time(NULL), program_start));
            ++perfect_number_found;
            updateLogWindow(log);
        }
        mpz_clear(num);
    }
    return NULL;
}