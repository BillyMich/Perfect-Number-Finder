#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <gmp.h>
#include "../include/task_queue.h"
#include "../include/perfect_number.h"
#include "../include/prime_number.h"
#include "../include/ui.h"

#define MAX_THREADS 16

pthread_t threads[MAX_THREADS];
time_t program_start;
mpz_t numberReading;
WINDOW *status_win, *log_win;
int perfect_number_found = 0;

void* pingCount() {
    while (1) {
        sleep(1);
        updateStatusWindow();
    }
    return NULL;
}

int main() {
    int numThreads;
    printf("Enter the number of threads: ");
    if (scanf("%d", &numThreads) != 1) {
        fprintf(stderr, "Error reading number of threads.\n");
        return 1;
    }
    program_start = time(NULL);
    initTaskQueue();

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

    destroyTaskQueue();

    return 0;
}