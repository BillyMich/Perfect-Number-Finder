#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define MAX_THREADS 16
#define QUEUE_SIZE 1000

typedef struct {
    long long int numbers[QUEUE_SIZE];
    int front;
    int rear;
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} TaskQueue;

TaskQueue queue;
pthread_t threads[MAX_THREADS];
clock_t program_start;

int isPerfectNumber(int num) {
    long long int sum = 1;
    long long int sqrtNum = sqrt(num);
    for (int i = 2; i <= sqrtNum; i++) {
        if (num % i == 0) {
            sum += i;
            if (i != num / i) {
                sum += num / i;
            }
        }
    }
    return sum == num && num != 1;
}

void* worker(void* arg) {
    while (1) {
        pthread_mutex_lock(&queue.mutex);
        while (queue.count == 0) {
            pthread_cond_wait(&queue.cond, &queue.mutex);
        }
        int num = queue.numbers[queue.front];
        queue.front = (queue.front + 1) % QUEUE_SIZE;
        queue.count--;
        pthread_mutex_unlock(&queue.mutex);

        if (isPerfectNumber(num)) {
            clock_t end = clock();
            double time_taken = ((double)(end - program_start)) / CLOCKS_PER_SEC;
            printf("%d is a perfect number. Time taken: %f seconds\n", num, time_taken);
        }
    }
    return NULL;
}

void enqueue(int num) {
    pthread_mutex_lock(&queue.mutex);
    queue.numbers[queue.rear] = num;
    queue.rear = (queue.rear + 1) % QUEUE_SIZE;
    queue.count++;
    pthread_cond_signal(&queue.cond);
    pthread_mutex_unlock(&queue.mutex);
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
    pthread_cond_init(&queue.cond, NULL);

    program_start = clock();

    for (int i = 0; i < numThreads; i++) {
        pthread_create(&threads[i], NULL, worker, NULL);
    }

    long long int n = 1;
    while (1) {
        enqueue(n++);
    }

    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&queue.mutex);
    pthread_cond_destroy(&queue.cond);

    return 0;
}
