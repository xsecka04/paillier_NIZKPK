#include <stdio.h>
#include <gmp.h>
#include "paillier.h"
#include "join.h"




int main() {

    paillier_prvkey_t* prv;
    paillier_pubkey_t* pub;
    Setup_SGM* setup;

    nizkpk_setup_SGM(256, &setup, &prv, &pub);
    gmp_printf ("Public key (n): %Zd\n", setup->pub.n);
    gmp_printf ("Blinding Factor: %Zd\n", setup->g.r);
    gmp_printf ("Private key (lambda): %Zd\n", prv->lambda);
    //printf("%d",setup->pub.bits);
    e1* e1 = generate_e1(setup);

    //gmp_printf ("Private key (lambda): %Zd\n", e1->e1.c);
    //gmp_printf ("Public key (n): %Zd\n", (*setup).pub.n);

    //gmp_printf("Parameter e1: %Zd\n", e1->e1);




}