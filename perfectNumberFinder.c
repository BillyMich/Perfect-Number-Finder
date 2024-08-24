#include <ncurses.h>
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

typedef struct {
    mpz_t numbers[QUEUE_SIZE];
    int front, rear, count;
    pthread_mutex_t mutex;
    pthread_cond_t cond_not_empty;
    pthread_cond_t cond_not_full;
} TaskQueue;

TaskQueue queue;
pthread_t threads[MAX_THREADS];
time_t program_start;
mpz_t numberReading;
WINDOW *status_win, *log_win;
int perfect_number_found = 0;

// Function Prototypes
void updateStatusWindow();
void updateLogWindow(const char* log);



int isPerfectNumber(mpz_t num) {
    mpz_t sum, sqrtNum, quotient, remainder;
    mpz_inits(sum, sqrtNum, quotient, remainder, NULL);
    mpz_set_ui(sum, 1);  // Start with 1 since it's a divisor for all numbers
    mpz_sqrt(sqrtNum, num);

    int early_exit = 0;
    time_t program_start = time(NULL);
    char log[256];

    // Use OpenMP to parallelize the loop
    #pragma omp parallel
    {
        mpz_t local_sum, local_i, local_quotient, local_remainder;
        mpz_inits(local_sum, local_i, local_quotient, local_remainder, NULL);
        mpz_set_ui(local_sum, 0);

        unsigned long long int i = mpz_get_ui(sqrtNum);

        #pragma omp for
        for (unsigned long long int j = 0; j < i - 1; j++) {
            if (early_exit) continue;

            unsigned long long int current_i = i - j;
            mpz_set_ui(local_i, current_i);
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
        sleep(1);
        updateStatusWindow();
    }
    return NULL;
}

void generatePerfectNumber(mpz_t result, mpz_t p) {
    mpz_t mersennePrime;
    mpz_init(mersennePrime);

    mpz_set_ui(result, 1);
    mpz_mul_2exp(result, result, mpz_get_ui(p));
    mpz_sub_ui(mersennePrime, result, 1);

    mpz_mul_2exp(result, mersennePrime, mpz_get_ui(p) - 1);

    mpz_clear(mersennePrime);
}


int isPrime(mpz_t num) {
    mpz_t i, sqrtNum, remainder;
    mpz_inits(i, sqrtNum, remainder, NULL);

    if (mpz_cmp_ui(num, 2) < 0) {
        mpz_clears(i, sqrtNum, remainder, NULL);
        return 0;
    }
    if (mpz_cmp_ui(num, 2) == 0) {
        mpz_clears(i, sqrtNum, remainder, NULL);
        return 1;
    }
    if (mpz_even_p(num)) {
        mpz_clears(i, sqrtNum, remainder, NULL);
        return 0;
    }

    mpz_sqrt(sqrtNum, num);
    for (mpz_set_ui(i, 3); mpz_cmp(i, sqrtNum) <= 0; mpz_add_ui(i, i, 2)) {
        mpz_mod(remainder, num, i);
        if (mpz_cmp_ui(remainder, 0) == 0) {
            mpz_clears(i, sqrtNum, remainder, NULL);
            return 0;
        }
    }

    mpz_clears(i, sqrtNum, remainder, NULL);
    return 1;
}

// Function to get the next prime number
void getNextPrime(mpz_t next, mpz_t current) {
    mpz_add_ui(next, current, 1);
    while (!isPrime(next)) {
        mpz_add_ui(next, next, 1);
    }
}

void updateStatusWindow() {
    wclear(status_win);
    box(status_win, 0, 0);
    mvwprintw(status_win, 1, 1, "Perfect Number Finder");
    mvwprintw(status_win, 2, 1, "Running time: %.2f seconds", difftime(time(NULL), program_start));
    char log[100256];
    gmp_snprintf(log, sizeof(log), "Count p: %Zd", numberReading);

    mvwprintw(status_win, 3, 1, "Perfect numbers found so far: %d ,%s", perfect_number_found, log);
    wrefresh(status_win);
}

void updateLogWindow(const char* log) {
    wprintw(log_win, "%s\n", log);
    wrefresh(log_win);
}

int main() {
    int numThreads;
    printf("Enter the number of threads: ");
    if (scanf("%d", &numThreads) != 1) {
        fprintf(stderr, "Error reading number of threads.\n");
        return 1;
    }
    program_start = time(NULL);
    queue.front = 0;
    queue.rear = 0;
    queue.count = 0;
    pthread_mutex_init(&queue.mutex, NULL);
    pthread_cond_init(&queue.cond_not_empty, NULL);
    pthread_cond_init(&queue.cond_not_full, NULL);


    initscr();
    cbreak();
    noecho();
    curs_set(FALSE);

    int height, width;
    getmaxyx(stdscr, height, width);

    status_win = newwin(5, width, 0, 0);
    log_win = newwin(height - 5, width, 5, 0);

    box(status_win, 0, 0);
    scrollok(log_win, TRUE);

    for (int i = 0; i < numThreads; i++) {
        pthread_create(&threads[i], NULL, worker, NULL);
    }

    pthread_t ping_thread;
    pthread_create(&ping_thread, NULL, pingCount, NULL);

    mpz_init_set_ui(numberReading, 2);

    while (1) {
        mpz_t perfectNumber;
        mpz_init(perfectNumber);

        if (!isPrime(numberReading)) {
            getNextPrime(numberReading, numberReading);
        }

        generatePerfectNumber(perfectNumber, numberReading);
        enqueue(perfectNumber);
        mpz_add_ui(numberReading, numberReading, 1); // Increment to the next number

        mpz_clear(perfectNumber);
    }

    mpz_clear(numberReading);

    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&queue.mutex);
    pthread_cond_destroy(&queue.cond_not_empty);
    pthread_cond_destroy(&queue.cond_not_full);

    return 0;
}