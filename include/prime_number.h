#ifndef PRIME_NUMBER_H
#define PRIME_NUMBER_H

#include <gmp.h>

int isPrime(mpz_t num);
void getNextPrime(mpz_t next, mpz_t current);

#endif // PRIME_NUMBER_H