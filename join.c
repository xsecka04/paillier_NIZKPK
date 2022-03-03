#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include "nizkpk_join.h"


int main() {

    // Values init

    int q_EC = 97;
    int g_EC = 96;
    int p_RSA = 101;
    int q_RSA = 103;


    Setup_SGM setup;
    Manager_S m_secret;
    generate_nizkpk_setup(&setup, &m_secret, q_EC, g_EC, p_RSA, q_RSA);

    E_1 e1 = generate_e1(&setup, &m_secret);

    Sender_S s_secret;
    E_2 e2 = generate_e2(&setup, &s_secret, &e1);


    //BEGIN MGR_STEP 5
    /* -----------------------------------------------------------------------*/

    // Decryption

    mpz_t dec;
    mpz_init(dec);
    mpz_powm(dec, e2.e2, m_secret.phi_n, setup.n2);
    mpz_sub_ui(dec, dec, 1);
    mpz_fdiv_q(dec, dec, setup.n);
    mpz_mod(dec, dec, setup.n2);

    mpz_t phi_inv;
    mpz_init(phi_inv);
    mpz_invert(phi_inv, m_secret.phi_n, setup.n);

    mpz_mul(dec, dec, phi_inv);
    mpz_mod(dec, dec, setup.n);
    gmp_printf("decrypted e2: %Zd\n", dec);

    mpz_t x;
    mpz_init(x);
    mpz_sub(x, dec, m_secret.n_half);
    gmp_printf("x: %Zd\n", x);


    /* -----------------------------------------------------------------------*/
    //END MGR_STEP 5

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


    if(mpz_cmp(dec, test) == 0){
        printf("Test conducted successfully\n");
    }
    else{
        printf("Test not conducted successfully\n");

    }

    gmp_printf("Sk_m: %Zd\n", m_secret.sk_m);
    gmp_printf("Sk_i: %Zd\n", s_secret.sk_i);


    gmp_printf("r: %Zd\n", m_secret.r);
    gmp_printf("r1: %Zd\n", s_secret.r1);
    gmp_printf("r2: %Zd\n", s_secret.r2);
    gmp_printf("r_bar: %Zd\n", s_secret.r_bar);

}
