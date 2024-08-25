#include <gmp.h>
#include "../include/prime_number.h"

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

void getNextPrime(mpz_t next, mpz_t current) {
    mpz_add_ui(next, current, 1);
    while (!isPrime(next)) {
        mpz_add_ui(next, next, 1);
    }
}