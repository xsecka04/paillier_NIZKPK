#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include "nizkpk_join.h"
#include <time.h>

#define TESTSNO 100

int main() {

    //char* q_EC = "29";
    //char* g_EC = "6";

    //char* q_EC = "2523648240000001ba344d8000000007ff9f800000000010a10000000000000d";
    char* g_EC = "2523648240000001BA344D80000000086121000000000013A7000000000000120000000000000000000000000000000000000000000000000000000000000001";


    //char* q_EC = "6D";
    //char* g_EC = "6";

    //char* q_EC = "11";
    //char* g_EC = "3";

    //int p_RSA = 37;
    //int q_RSA = 47;
    //int p_RSA = 3971679581;
    //int q_RSA = 2505093049;

    clock_t start, end, setup_stop, join_start;
    double cpu_time_used, cpu_time_used_setup, cpu_time_used_join;

    start = clock();


    char* q_EC = "2523648240000001ba344d8000000007ff9f800000000010a10000000000000d";


    int verify;
    int kappa = 3;
    Setup_SGM setup;
    Manager_S m_secret;
    generate_nizkpk_setup(&setup, &m_secret, q_EC, g_EC, kappa);
 
    setup_stop = clock();

    Sender_S s_secrets[TESTSNO];
    E_1 e1s[TESTSNO];
    E_2 e2s[TESTSNO];
    Sig_star sigs[TESTSNO];

    for(int i=0;i<TESTSNO;i++){
        e1s[i] = generate_e1(&setup, &m_secret);
        e2s[i] = generate_e2(&setup, &s_secrets[i], &e1s[i], kappa);
        sigs[i] = decrypt_e2(&setup, &m_secret, &e2s[i]);
        verify = verify_sig(&sigs[i], &m_secret, &s_secrets[i], &setup);

        if(verify == 1){
            printf("ERROR: Test NOT conducted successfully\n");
        }

    }

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    cpu_time_used_setup = ((double) (setup_stop - start)) / CLOCKS_PER_SEC;
    cpu_time_used_join = ((double) (end - setup_stop)) / CLOCKS_PER_SEC;

    printf("Elapsed time after establishing %d sender(s): %f ms\n", TESTSNO, cpu_time_used*1000);
    printf("Elapsed time for Setup_SGM algorithm: %f ms\n", cpu_time_used_setup*1000);
    printf("Elapsed time for Join algorithm : %f ms\n", TESTSNO, cpu_time_used_join*1000);

}