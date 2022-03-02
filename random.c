#include <stdio.h>
#include <gmp.h>
#include "paillier.h"

void test() {
    

    int n = 3024121;

    mpz_t r;
    mpz_init(r);

    gmp_randstate_t rand;
    gmp_randinit_default(rand);

    mpz_t group;
    mpz_init(group);
    mpz_set_ui(group, n);

    mpz_t ord;
    mpz_init(ord);
    mpz_set_ui(ord, 1656);

    mpz_t i;
    mpz_init(i);
    mpz_set_ui(i, 1);

    mpz_t j;
    mpz_init(j);

    do{
        mpz_urandomm(r, rand, group);
        mpz_powm(j, r, ord, group);
    }while( mpz_cmp(j, i) != 0 );

    mpz_clear(group);
    mpz_clear(ord);
    mpz_clear(i);
    mpz_clear(j);

    gmp_printf("random val: %Zd\n", r);
    mpz_clear(r);

}

void gmptest(mpz_t* test){

    gmp_printf("test: %Zd\n", test);


}


int main() {

    // Values init

    int q1 = 71;

    mpz_t q;
    mpz_init(q);
    mpz_set_ui(q, q1);

    mpz_t g_q;
    mpz_init(g_q);
    mpz_set_ui(g_q, 7);


    mpz_t p_n;
    mpz_init(p_n);
    mpz_set_ui(p_n, 71);
    mpz_t q_n;
    mpz_init(q_n);
    mpz_set_ui(q_n, 73);

    mpz_t n;
    mpz_init(n);
    mpz_mul(n, p_n, q_n);
    mpz_t g;
    mpz_init(g);
    mpz_set_ui(g, 20251867);
    mpz_t h;
    mpz_init(h);
    mpz_add_ui(h, n, 1);


    mpz_t n2;
    mpz_init(n2);
    mpz_pow_ui(n2, n, 2);
    gmp_printf("n2: %Zd\n", n2);


    mpz_t skm;
    mpz_init(skm);
    mpz_set_ui(skm, 10);
    mpz_t ski;
    mpz_init(ski);
    mpz_set_ui(ski, 11);


    mpz_t two;
    mpz_init(two);
    mpz_set_ui(two, 2);

    mpz_t n_half;
    mpz_init(n_half);
    mpz_fdiv_q(n_half, n, two);
    gmp_printf("n_half: %Zd\n", n_half);


    mpz_t r;
    mpz_init(r);
    mpz_set_ui(r, 5);

    mpz_t r1;
    mpz_init(r1);
    mpz_set_ui(r1, 7);

    mpz_t r2;
    mpz_init(r2);
    mpz_set_ui(r2, 8);

    mpz_t r_bar;
    mpz_init(r_bar);
    mpz_set_ui(r_bar, 6);

    mpz_t e11;
    mpz_init(e11);

    // e1 calc

    mpz_t e1;
    mpz_init(e1);
    mpz_add(e1, n_half, skm);
    mpz_powm(e1, h, e1, n2);
    mpz_powm(e11, g, r, n2);
    mpz_mul(e1, e1, e11);
    mpz_mod(e1, e1, n2);
    gmp_printf("e1: %Zd\n", e1);

    // e2 calc

    mpz_t e2;
    mpz_init(e2);

    mpz_t e22;
    mpz_init(e22);

    mpz_t e23;
    mpz_init(e23);


    mpz_powm(e23, e1, r1, n2);
    
    mpz_mul(e2, ski, r1);
    mpz_mul(e22, r2, q);
    mpz_add(e2, e2, e22);
    mpz_powm(e2, h, e2, n2);

    mpz_powm(e22, g, r_bar, n2);
    
    mpz_mul(e2, e2, e23);
    mpz_mul(e2, e2, e22);
    mpz_mod(e2, e2, n2);
    gmp_printf("e2: %Zd\n", e2);

    // Decryption

    mpz_t phi_n;
    mpz_init(phi_n);
    mpz_sub_ui(p_n, p_n, 1);
    mpz_sub_ui(q_n, q_n, 1);
    mpz_mul(phi_n, p_n, q_n);

    mpz_t phi_n2;
    mpz_init(phi_n2);
    mpz_mul(phi_n2, phi_n, n);

    mpz_t dec;
    mpz_init(dec);
    mpz_powm(dec, e2, phi_n, n2);
    mpz_sub_ui(dec, dec, 1);
    mpz_fdiv_q(dec, dec, n);
    mpz_mod(dec, dec, n2);

    mpz_t phi_inv;
    mpz_init(phi_inv);
    mpz_invert(phi_inv, phi_n, n);

    mpz_mul(dec, dec, phi_inv);
    mpz_mod(dec, dec, n);
    gmp_printf("decrypted e2: %Zd\n", dec);

    mpz_t test;
    mpz_init(test);
    mpz_add(test, skm, ski);
    mpz_add(test, test, n_half);
    mpz_mul(test, test, r1);

    mpz_t test2;
    mpz_init(test2);
    mpz_mul(test2, q, r2);

    mpz_add(test, test, test2);
    mpz_mod(test, test, n);
    gmp_printf("test: %Zd\n", test);

    if(mpz_cmp(dec, test) == 0){
        printf("Congratulations , VELKE LOVE TA CAKAJU\n");
    }















}