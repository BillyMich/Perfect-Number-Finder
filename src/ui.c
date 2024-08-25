#include <ncurses.h>
#include <time.h>
#include <gmp.h>
#include "../include/ui.h"

extern time_t program_start;
extern WINDOW *status_win, *log_win;
extern int perfect_number_found;
extern mpz_t numberReading;

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