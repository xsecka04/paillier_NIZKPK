#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include "nizkpk_join.h"


int main() {

    // Values init

    //char* q_EC = "2523648240000001ba344d8000000007ff9f800000000010a10000000000000d";
    char* q_EC = "16098B2CD";


    //char* g_EC = "042523648240000001BA344D80000000086121000000000013A7000000000000120000000000000000000000000000000000000000000000000000000000000001";
    char* g_EC = "7";

    int p_RSA = 101;
    int q_RSA = 103;


    Setup_SGM setup;
    Manager_S m_secret;
    generate_nizkpk_setup(&setup, &m_secret, q_EC, g_EC, p_RSA, q_RSA);
    gmp_printf("q: %Zd\n", setup.q_EC);
    gmp_printf("gen: %Zd\n", setup.g_EC);

    E_1 e1 = generate_e1(&setup, &m_secret);

    Sender_S s_secret;
    E_2 e2 = generate_e2(&setup, &s_secret, &e1);

    Sig_star sig = decrypt_e2(&setup, &m_secret, &e2);



    //BEGIN SEND_STEP 6
    /* -----------------------------------------------------------------------*/

    //Test

    mpz_t test;
    mpz_init(test);
    mpz_add(test, m_secret.sk_m, s_secret.sk_i);
    mpz_add(test, test, m_secret.n_half);
    mpz_mul(test, test, s_secret.r1);

    mpz_t test2;
    mpz_init(test2);
    mpz_mul(test2, setup.q_EC, s_secret.r2);

    mpz_add(test, test, test2);
    mpz_mod(test, test, setup.n);
    gmp_printf("test: %Zd\n", test);

    /* -----------------------------------------------------------------------*/
    //END SEND_STEP 6

    if(mpz_cmp(sig.sig_star, test) == 0){
        printf("Test conducted successfully\n");
    }
    else{
        printf("ERROR: Test NOT conducted successfully\n");

    }

    gmp_printf("Sk_m: %Zd\n", m_secret.sk_m);
    gmp_printf("Sk_i: %Zd\n", s_secret.sk_i);


    gmp_printf("r: %Zd\n", m_secret.r);
    gmp_printf("r1: %Zd\n", s_secret.r1);
    gmp_printf("r2: %Zd\n", s_secret.r2);
    gmp_printf("r_bar: %Zd\n", s_secret.r_bar);

}
