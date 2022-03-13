#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include "nizkpk_join.h"


int main() {

    // Values init

    char* g_EC = "2523648240000001BA344D80000000086121000000000013A7000000000000120000000000000000000000000000000000000000000000000000000000000001";



    //char* q_EC = "74";
    //char* g_EC = "7";

    //char* q_EC = "6D";
    //char* g_EC = "6";

    //char* q_EC = "11";
    //char* g_EC = "3";

    int p_RSA = 3;
    int q_RSA = 5;
    //int p_RSA = 3971679581;
    //int q_RSA = 2505093049;






    int kappa = 3;
    //char* q_EC = "2523648240000001ba344d8000000007ff9f800000000010a10000000000000d";
    char* q_EC = "74";

    Setup_SGM setup;
    Manager_S m_secret;
    generate_nizkpk_setup(&setup, &m_secret, q_EC, g_EC, kappa);

    E_1 e1 = generate_e1(&setup, &m_secret);


    Sender_S s_secret;
    E_2 e2 = generate_e2(&setup, &s_secret, &e1, kappa);


    Sig_star sig = decrypt_e2(&setup, &m_secret, &e2);


    int verify = verify_sig(&sig, &m_secret, &s_secret, &setup);
    if(verify == 1){
        printf("ERROR: Test NOT conducted successfully\n");
    }

    gmp_printf("q: %Zd\n", setup.q_EC);
    gmp_printf("gen: %Zd\n", setup.g_EC);


    gmp_printf("Sk_m: %Zd\n", m_secret.sk_m);
    gmp_printf("Sk_i: %Zd\n", s_secret.sk_i);


    gmp_printf("r: %Zd\n", m_secret.r);
    gmp_printf("r1: %Zd\n", s_secret.r1);
    gmp_printf("r2: %Zd\n", s_secret.r2);
    gmp_printf("r_bar: %Zd\n", s_secret.r_bar);

}
