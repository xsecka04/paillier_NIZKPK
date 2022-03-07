#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include "nizkpk_join.h"


void test(mpz_t* primes){

    mpz_init(primes[0]);
    mpz_set_ui(primes[0], 10);
}



int main() {

    mpz_t * primes;
    primes = malloc(sizeof(mpz_t) * 10);
    test(primes);


    gmp_printf("Prime divisor of %Zd\n",primes[0]);

    mpz_t n2;
    mpz_t phi_n2;
    mpz_t phi_n;
    mpz_t g;

    mpz_inits(n2,phi_n2,phi_n,g, NULL);


    mpz_set_ui(n2, 14);
    mpz_set_ui(phi_n2, 6);
    mpz_set_ui(phi_n, 4);


    generate_g(&n2, &phi_n2, &phi_n, &g);



}