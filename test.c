#include <stdio.h>
#include <gmp.h>
#include "paillier.h"
#include "join.h"
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 8080
#define BUFFER_LENGTH 1000



int main() {

    paillier_prvkey_t* prv;
    paillier_pubkey_t* pub;

    Setup_SGM* setup = nizkpk_setup_SGM(80, prv, pub);
    e1* e1 = generate_e1(setup);

    gmp_printf ("Private key (lambda): %Zd\n", prv->lambda);
    gmp_printf ("Public key (n): %Zd\n", (*setup).pub.n);

    gmp_printf("Parameter e1: %Zd\n", e1->e1);




}