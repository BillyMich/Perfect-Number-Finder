#ifndef PERFECT_NUMBER_H
#define PERFECT_NUMBER_H

#include <gmp.h>

int isPerfectNumber(mpz_t num);
void generatePerfectNumber(mpz_t result, mpz_t p);

#endif // PERFECT_NUMBER_H