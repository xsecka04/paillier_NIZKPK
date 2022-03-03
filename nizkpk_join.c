#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include "nizkpk_join.h"

void
get_rand_seed( void* buf, int len)
{
	FILE* fp;
	void* p;

	fp = fopen("/dev/urandom", "r");

	p = buf;
	while( len )
	{
		size_t s;
		s = fread(p, 1, len, fp);
		p += s;
		len -= s;
	}

	fclose(fp);
}


void generate_g(mpz_t* n2, mpz_t* ord, mpz_t* g) {
    
    unsigned long s;
    gmp_randstate_t rand;
    gmp_randinit_default(rand);
    gmp_randseed_ui(rand, s);

    mpz_t i;
    mpz_init(i);
    mpz_set_ui(i, 1);

    mpz_t j;
    mpz_init(j);

    do{
        mpz_urandomm(*g, rand, *n2);
        mpz_powm(j, *g, *ord, *n2);
    }while( mpz_cmp(j, i) != 0 );

    mpz_clear(i);
    mpz_clear(j);

    gmp_printf("g: %Zd\n", g);

    gmp_randclear(rand);

}

void generate_r_from_group(mpz_t* mod, mpz_t* r){
    
    int len = 15;
    mpz_t s;
	mpz_init(s);
    void* buf;
	buf = malloc(len);
	get_rand_seed(buf, len);
	mpz_import(s, 15, 1, 1, 0, 0, buf);

    gmp_randstate_t rand;
    gmp_randinit_default(rand);
    gmp_randseed(rand, s);

    mpz_urandomm(*r, rand, *mod);
    
    gmp_randclear(rand);
    mpz_clear(s);

}


void generate_nizkpk_setup(Setup_SGM* setup, Manager_S* m_secret, int q_EC, int g_EC, int p_RSA, int q_RSA){
    
    //BEGIN MGR_SETUP
    /* -----------------------------------------------------------------------*/

    //TODO: EC params
    mpz_init(setup->q_EC);
    mpz_set_ui(setup->q_EC, q_EC);

    mpz_init(setup->g_EC);
    mpz_set_ui(setup->g_EC, g_EC);

    //TODO: RSA params generation impl
    mpz_t p_n;
    mpz_init(p_n);
    mpz_set_ui(p_n, p_RSA);
    mpz_t q_n;
    mpz_init(q_n);
    mpz_set_ui(q_n, q_RSA);

    //Secrets impl
    mpz_init(m_secret->sk_m);
    generate_r_from_group(&setup->q_EC, &m_secret->sk_m);


    mpz_inits(setup->g, setup->n, setup->h, setup->n2, m_secret->phi_n, NULL);
    
    mpz_mul(setup->n, p_n, q_n);
    mpz_add_ui(setup->h, setup->n, 1);
    mpz_pow_ui(setup->n2, setup->n, 2);
    mpz_sub_ui(p_n, p_n, 1);
    mpz_sub_ui(q_n, q_n, 1);
    mpz_mul(m_secret->phi_n, p_n, q_n);

    //TODO: Seems redundant
    mpz_init(m_secret->phi_n2);
    mpz_mul(m_secret->phi_n2, m_secret->phi_n, setup->n);

    mpz_t two;
    mpz_init(two);
    mpz_set_ui(two, 2);

    mpz_init(m_secret->n_half);
    mpz_fdiv_q(m_secret->n_half, setup->n, two);
    gmp_printf("n_half: %Zd\n", m_secret->n_half);

    
    //G generation
    generate_g(&setup->n2, &m_secret->phi_n, &setup->g);

    mpz_clears(two, p_n, q_n, NULL);


    /* -----------------------------------------------------------------------*/
    //END MGR_SETUP

}

E_1 generate_e1(Setup_SGM* setup, Manager_S* m_secret){

        //BEGIN MGR_STEP 1
    /* -----------------------------------------------------------------------*/
    E_1 e1;
    //Random variables impl
    mpz_init(m_secret->r);
    generate_r_from_group(&m_secret->phi_n, &m_secret->r);

    mpz_t e11;
    mpz_init(e11);

    //e1 calc
    mpz_init(e1.e1);
    mpz_add(e1.e1, m_secret->n_half, m_secret->sk_m);
    mpz_powm(e1.e1, setup->h, e1.e1, setup->n2);
    mpz_powm(e11, setup->g, m_secret->r, setup->n2);
    mpz_mul(e1.e1, e1.e1, e11);
    mpz_mod(e1.e1, e1.e1, setup->n2);
    gmp_printf("e1: %Zd\n", e1.e1);
    printf("Hellp");
    return e1;

    /* -----------------------------------------------------------------------*/
    //END MGR_STEP 1
}

E_2 generate_e2(Setup_SGM* setup, Sender_S* s_secret, E_1* e1){

    //BEGIN SEND_STEP 3
    /* -----------------------------------------------------------------------*/
    E_2 e2;
    //Secrets impl
    mpz_inits(s_secret->sk_i, s_secret->r1, s_secret->r2, s_secret->r_bar, NULL);

    generate_r_from_group(&setup->q_EC, &s_secret->sk_i);
    generate_r_from_group(&setup->q_EC, &s_secret->r1);
    generate_r_from_group(&setup->q_EC, &s_secret->r2);
    generate_r_from_group(&setup->n, &s_secret->r_bar);
    
    // e2 calc
    mpz_init(e2.e2);

    mpz_t e22, e23;
    mpz_inits(e22, e23, NULL);

    mpz_powm(e23, e1->e1, s_secret->r1, setup->n2);

    mpz_mul(e2.e2, s_secret->sk_i, s_secret->r1);
    mpz_mul(e22, s_secret->r2, setup->q_EC);
    mpz_add(e2.e2, e2.e2, e22);
    mpz_powm(e2.e2, setup->h, e2.e2, setup->n2);

    mpz_powm(e22, setup->g, s_secret->r_bar, setup->n2);
    
    mpz_mul(e2.e2, e2.e2, e23);
    mpz_mul(e2.e2, e2.e2, e22);
    mpz_mod(e2.e2, e2.e2, setup->n2);
    gmp_printf("e2: %Zd\n", e2.e2);

    mpz_clears(e22, e23, NULL);

    return e2;
    /* -----------------------------------------------------------------------*/
    //END SEND_STEP 3

}
