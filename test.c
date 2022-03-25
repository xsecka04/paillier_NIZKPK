#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include "lib/nizkpk_join.h"
#include <time.h>

#define TESTSNO 1

double mean(double arr[10]){

    double sum = 0;
    double mean = 0;
    
    for(int i=0;i<10;i++){
        sum += arr[i];
    }
    mean = sum/10.f;
    return mean;
}

int main() {

    clock_t start, end, setup_stop, join_start, rpi_start, rpi_stop;
    double cpu_time_used, cpu_time_used_setup, cpu_time_used_join, cpu_time_used_join_sender, rpi_total;

    double time_setup[10];
    double time_join[10];
    double time_join_sender[10];
    double time_total[10];

    for(int i=0;i<10;i++){

            rpi_total = 0;

            start = clock();
            char* q_EC = "2523648240000001ba344d8000000007ff9f800000000010a10000000000000d";


            int verify;
            int kappa = 3;
            Setup_SGM setup;
            Manager_S m_secret;
            generate_nizkpk_setup(&setup, &m_secret, q_EC);
        
            setup_stop = clock();

            Sender_S s_secrets[TESTSNO];
            E_1 e1s[TESTSNO];
            E_2 e2s[TESTSNO];
            Sig_star sigs[TESTSNO];

            for(int i=0;i<TESTSNO;i++){
                e1s[i] = generate_e1(&setup, &m_secret);
                rpi_start = clock();
                e2s[i] = generate_e2(&setup, &s_secrets[i], &e1s[i]);
                rpi_stop = clock();
                sigs[i] = decrypt_e2(&setup, &m_secret, &e2s[i]);
                verify = verify_sig(&sigs[i], &m_secret, &s_secrets[i], &setup);
                rpi_total += ((double) rpi_stop - rpi_start);
                if(verify == 1){
                    printf("ERROR: Test NOT conducted successfully\n");
                }

            }

            end = clock();
            time_total[i] = ((double) (end - start)) / CLOCKS_PER_SEC;
            time_setup[i] = ((double) (setup_stop - start)) / CLOCKS_PER_SEC;
            time_join[i] = ((double) (end - setup_stop)) / CLOCKS_PER_SEC;
            time_join_sender[i] = rpi_total / CLOCKS_PER_SEC;

    }
    

    printf("Elapsed time after establishing %d sender(s): %f ms\n", TESTSNO, mean(time_total)*1000);
    printf("Elapsed time for Setup_SGM algorithm: %f ms\n", mean(time_setup)*1000);
    printf("Elapsed time for Join algorithm : %f ms\n", mean(time_join)*1000);
    printf("Elapsed time for Join algorithm on Sender : %f ms\n", mean(time_join_sender)*1000);

}