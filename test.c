#include <stdio.h>
#include <gmp.h>
#include "paillier.h"
#include "join.h"




int main() {

    paillier_prvkey_t* prv;
    paillier_pubkey_t* pub;
    Setup_SGM* setup;



    nizkpk_setup_SGM(256, &setup, &prv, &pub);



    // Generate SKm
    //mpz_t skm;
    //mpz_init(skm);
    //gmp_randstate_t rand;
    //init_rand(rand, paillier_get_rand_devurandom, (*setup).pub.bits / 8 + 1);
    //do
    //  mpz_urandomb(skm, rand, (*setup).pub.bits);
    //while( mpz_cmp(skm, (*setup).pub.n) >= 0 );

    //mpz_t two;
    //mpz_init(two);
    //mpz_set_ui(two, 10000);
    //mpz_mod(skm, skm, two);



    gmp_printf ("Public key (n): %Zd\n", setup->pub.n);
    gmp_printf ("Blinding Factor: %Zd\n", setup->g.r);
    gmp_printf ("Private key (lambda): %Zd\n", prv->lambda);
    //printf("%d",setup->pub.bits);
    e1* e1 = generate_e1(setup, prv);

    //gmp_printf ("Private key (lambda): %Zd\n", e1->e1.c);
    //gmp_printf ("Public key (n): %Zd\n", (*setup).pub.n);

    //gmp_printf("Parameter e1: %Zd\n", e1->e1);




}