#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <gmp.h>
#include <omp.h>


#define MAX_THREADS 16
#define QUEUE_SIZE 1

long long int nuberReading = 1;

typedef struct {
    mpz_t numbers[QUEUE_SIZE];
    int front, rear, count;
    pthread_mutex_t mutex;
    pthread_cond_t cond_not_empty;
    pthread_cond_t cond_not_full;
} TaskQueue;

TaskQueue queue;
pthread_t threads[MAX_THREADS];
clock_t program_start;

int isPerfectNumber(mpz_t num) {
    mpz_t sum, sqrtNum, quotient, remainder;
    mpz_inits(sum, sqrtNum, quotient, remainder, NULL);

    mpz_set_ui(sum, 1);  // Start with 1 since it's a divisor for all numbers
    mpz_sqrt(sqrtNum, num);

    int early_exit = 0;

    // Use OpenMP to parallelize the loop
    #pragma omp parallel
    {
        mpz_t local_sum, local_i, local_quotient, local_remainder;
        mpz_inits(local_sum, local_i, local_quotient, local_remainder, NULL);
        mpz_set_ui(local_sum, 0);

        #pragma omp for
        for (unsigned long long int i = 2; i <= mpz_get_ui(sqrtNum); i++) {
            if (early_exit) continue;

            mpz_set_ui(local_i, i);
            mpz_mod(local_remainder, num, local_i);
            if (mpz_cmp_ui(local_remainder, 0) == 0) {
                mpz_div(local_quotient, num, local_i);
                mpz_add(local_sum, local_sum, local_i);
                if (mpz_cmp(local_quotient, local_i) != 0) {
                    mpz_add(local_sum, local_sum, local_quotient);
                }
                if (mpz_cmp(local_sum, num) > 0) {
                    early_exit = 1;
                }
            }
        }

        #pragma omp critical
        mpz_add(sum, sum, local_sum);

        mpz_clears(local_sum, local_i, local_quotient, local_remainder, NULL);
    }

    int result = (early_exit == 0 && mpz_cmp(sum, num) == 0);
    mpz_clears(sum, sqrtNum, quotient, remainder, NULL);
    return result;
}

void* worker(void* arg) {
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
            clock_t end = clock();
            double time_taken = ((double)(end - program_start)) / CLOCKS_PER_SEC;
            gmp_printf("%Zd is a perfect number. Time taken: %.2f seconds\n", num, time_taken);
        }
        mpz_clear(num);
    }
    return NULL;
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

void* pingCount(void* arg) {
    while (1) {
        sleep(5);
        printf("Count of numbers read: %lld\n", nuberReading);
        printf("Count of numbers in queue: %d\n", queue.count);
    }
    return NULL;
}

void generatePerfectNumber(mpz_t result, long long int p) {
    mpz_t mersennePrime;
    mpz_init(mersennePrime);

    mpz_set_ui(result, 1);
    mpz_mul_2exp(result, result, p);
    mpz_sub_ui(mersennePrime, result, 1);

    mpz_mul_2exp(result, mersennePrime, p - 1);
    gmp_printf("New sample to see if is Perfect number: %Zd\n", mersennePrime);

    mpz_clear(mersennePrime);
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
        mpz_t perfectNumber;
        mpz_init(perfectNumber);
        generatePerfectNumber(perfectNumber, nuberReading++);
        enqueue(perfectNumber);
        mpz_clear(perfectNumber);
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