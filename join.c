#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include "lib/nizkpk_join.h"


int main() {

    // Values init
    char* q_EC = "2523648240000001ba344d8000000007ff9f800000000010a10000000000000d";
    //char* q_EC = "74";

    Setup_SGM setup;
    Manager_S m_secret;

    generate_nizkpk_setup(&setup, &m_secret, q_EC);

    JSON_serialize_Setup_par(&setup);
    Setup_SGM setup2;
    JSON_deserialize_Setup_par(&setup2);

    
    E_1 e1 = generate_e1(&setup, &m_secret);

    JSON_serialize_e1(&e1);
    E_1 e11;
    JSON_deserialize_e1(&e11);


    Sender_S s_secret;
    E_2 e2 = generate_e2(&setup2, &s_secret, &e11);
    
    JSON_serialize_e2(&e2);
    E_2 e22;
    JSON_deserialize_e2(&e22);


    Sig_star sig = decrypt_e2(&setup2, &m_secret, &e22);
    
    JSON_serialize_sig_star(&sig);
    Sig_star sig2;
    JSON_deserialize_sig_star(&sig2);

    int verify = verify_sig(&sig2, &m_secret, &s_secret, &setup2);
    if(verify == 1){
        printf("ERROR: Test NOT conducted successfully\n");
    }

    gmp_printf("q: %Zd\n", setup.q_EC);


    //gmp_printf("Sk_m: %Zd\n", m_secret.sk_m);
    //gmp_printf("Sk_i: %Zd\n", s_secret.sk_i);


    //gmp_printf("r: %Zd\n", m_secret.r);
    //gmp_printf("r1: %Zd\n", s_secret.r1);
    //gmp_printf("r2: %Zd\n", s_secret.r2);
    //gmp_printf("r_bar: %Zd\n", s_secret.r_bar);

}
