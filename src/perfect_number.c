#include <gmp.h>
#include <omp.h>
#include <time.h>
#include "../include/perfect_number.h"
#include "../include/ui.h"

extern time_t program_start;
extern int perfect_number_found;

int isPerfectNumber(mpz_t num) {
    mpz_t sum, sqrtNum, quotient, remainder;
    mpz_inits(sum, sqrtNum, quotient, remainder, NULL);
    mpz_set_ui(sum, 1); 
    mpz_sqrt(sqrtNum, num);

    int early_exit = 0;


        mpz_t local_sum, local_i, local_quotient, local_remainder;
        mpz_inits(local_sum, local_i, local_quotient, local_remainder, NULL);
        mpz_set_ui(local_sum, 0);

        unsigned long long int i = mpz_get_ui(sqrtNum);

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
        mpz_add(sum, sum, local_sum);

        mpz_clears(local_sum, local_i, local_quotient, local_remainder, NULL);
    
    int result = (early_exit == 0 && mpz_cmp(sum, num) == 0);
    mpz_clears(sum, sqrtNum, quotient, remainder, NULL);
    return result;
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