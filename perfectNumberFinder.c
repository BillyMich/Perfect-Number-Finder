#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#define MAX_THREADS 16
#define QUEUE_SIZE 1000

long long int nuberReading = 0;


typedef struct {
    long long int numbers[QUEUE_SIZE];
    int front;
    int rear;
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t cond_not_empty;
    pthread_cond_t cond_not_full;
} TaskQueue;

TaskQueue queue;
pthread_t threads[MAX_THREADS];
clock_t program_start;

int isPerfectNumber(long long int num) {

    long long int sum = 1;
    long long int sqrtNum = sqrt(num);

    for (int i = 2; i <= sqrtNum; i++) {
        if (num % i == 0) {
            int quotient = num / i;
            sum += i;
            if (quotient != i) {  // Avoid adding the square root twice if `i * i == num`
                sum += quotient;
            }
        }
    }

    return sum == num;
}

void* worker(void* arg) {
    while (1) {
        pthread_mutex_lock(&queue.mutex);
        while (queue.count == 0) {
            pthread_cond_signal(&queue.cond_not_full);
            pthread_cond_wait(&queue.cond_not_empty, &queue.mutex);
        }
        long long int num = queue.numbers[queue.front];
        queue.front = (queue.front + 1) % QUEUE_SIZE;
        queue.count--;
        pthread_mutex_unlock(&queue.mutex);

        if (isPerfectNumber(num)) {
            clock_t end = clock();
            double time_taken = ((double)(end - program_start)) / CLOCKS_PER_SEC;
            printf("%lld is a perfect number. Time taken: %f seconds\n", num, time_taken);
        }
    }
    return NULL;
}

void enqueue(long long int num) {
    pthread_mutex_lock(&queue.mutex);
    while (queue.count == QUEUE_SIZE) {
        pthread_cond_wait(&queue.cond_not_full, &queue.mutex);
    }
    queue.numbers[queue.rear] = num;
    queue.rear = (queue.rear + 1) % QUEUE_SIZE;
    queue.count++;
    pthread_cond_signal(&queue.cond_not_empty);
    pthread_mutex_unlock(&queue.mutex);
}

void* pingCount(void* arg) {
    while (1) {
        sleep(10);
        printf("Count of numbers read: %lld\n", nuberReading);
    }
    return NULL;
}

int main() {
    int numThreads;
    printf("Enter the number of threads: ");
    scanf("%d", &numThreads);
    if (numThreads > MAX_THREADS) {
        numThreads = MAX_THREADS;
    }

    queue.front = 0;
    queue.rear = 0;
    queue.count = 0;
    pthread_mutex_init(&queue.mutex, NULL);
    pthread_cond_init(&queue.cond_not_empty, NULL);
    pthread_cond_init(&queue.cond_not_full, NULL);

    program_start = clock();

    for (int i = 0; i < numThreads; i++) {
        pthread_create(&threads[i], NULL, worker, NULL);
    }

    pthread_t ping_thread;
    pthread_create(&ping_thread, NULL, pingCount, NULL);

    while (1) {
        nuberReading+=2;
        enqueue(nuberReading);
    }

    for (int i = 0; i < numThreads; i++) {
        pthread_cancel(threads[i]);
        pthread_join(threads[i], NULL);
    }

    pthread_cancel(ping_thread);
    pthread_join(ping_thread, NULL);

    pthread_mutex_destroy(&queue.mutex);
    pthread_cond_destroy(&queue.cond_not_empty);
    pthread_cond_destroy(&queue.cond_not_full);

    return 0;
}