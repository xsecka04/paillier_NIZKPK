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
        gmp_printf("g: %Zd\n", *g);
        mpz_powm(j, *g, *ord, *n2);
        gmp_printf("j: %Zd\n", j);
    }while( mpz_cmp(j, i) != 0 );

    mpz_clear(i);
    mpz_clear(j);

    gmp_printf("g: %Zd\n", g);

    gmp_randclear(rand);

}

void generate_r_from_group(mpz_t* mod, mpz_t* r){
    
    int len = 512;
    mpz_t s;
	mpz_init(s);
    void* buf;
	buf = malloc(len);
	get_rand_seed(buf, len);
	mpz_import(s, len, 1, 1, 0, 0, buf);

    gmp_randstate_t rand;
    gmp_randinit_default(rand);
    gmp_randseed(rand, s);

    mpz_urandomm(*r, rand, *mod);
    
    gmp_randclear(rand);
    mpz_clear(s);

}




void generate_nizkpk_setup(Setup_SGM* setup, Manager_S* m_secret, char* q_EC, char* g_EC, int p_RSA, int q_RSA){
    
    //BEGIN MGR_SETUP
    /* -----------------------------------------------------------------------*/

    //EC params
    mpz_inits(setup->q_EC, setup->g_EC, NULL);
    //mpz_set_ui(setup->q_EC, q_EC);
    mpz_set_str(setup->q_EC, q_EC, 16);
    //mpz_set_ui(setup->g_EC, g_EC);
    mpz_set_str(setup->g_EC, g_EC, 16);



    //TODO: RSA params generation impl
    mpz_t p_n;
    mpz_init(p_n);
    mpz_set_ui(p_n, p_RSA);
    mpz_t q_n;
    mpz_init(q_n);
    mpz_set_ui(q_n, q_RSA);

    
    mpz_t q2;
    mpz_init(q2);
    mpz_pow_ui(q2, setup->q_EC, 2);

    size_t size = mpz_sizeinbase(q2, 2);
    gmp_printf("q2: %Zd\n", q2);
    printf("size of q2 in bits: %zu\n", size/2);

    mpz_t s;
	mpz_init(s);
    void* buf;
	buf = malloc(size);
	get_rand_seed(buf, size);
	mpz_import(s, size, 1, 1, 0, 0, buf);

    gmp_randstate_t rand;
    gmp_randinit_default(rand);
    gmp_randseed(rand, s);
    
    
    mpz_init(setup->n);
	do
	{
		do
			mpz_urandomb(p_n, rand, size/2 + 1);
		while( !mpz_probab_prime_p(p_n, 10) );

		do
			mpz_urandomb(q_n, rand, size/2 + 1);
		while( !mpz_probab_prime_p(q_n, 10) );
    
        mpz_mul(setup->n, p_n, q_n);

	//} while( mpz_cmp(setup->n, q2) < 0 );
	} while( !mpz_tstbit(setup->n, size - 1) );

    gmp_printf("p_n: %Zd\n", p_n);
    gmp_printf("q_n: %Zd\n", q_n);
    gmp_printf("setup->n: %Zd\n", setup->n);

    gmp_randclear(rand);
    mpz_clear(s);
    


    //mpz_init(setup->n);



    //Secrets impl
    mpz_init(m_secret->sk_m);
    generate_r_from_group(&setup->q_EC, &m_secret->sk_m);


    mpz_inits(setup->g, setup->h, setup->n2, m_secret->phi_n, NULL);
    
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

Sig_star decrypt_e2(Setup_SGM* setup, Manager_S* m_secret, E_2* e2){

    //BEGIN MGR_STEP 5
    /* -----------------------------------------------------------------------*/

    Sig_star sig_star;

    // Decryption

    //mpz_t dec;
    mpz_init(sig_star.sig_star);
    mpz_powm(sig_star.sig_star, e2->e2, m_secret->phi_n, setup->n2);
    mpz_sub_ui(sig_star.sig_star, sig_star.sig_star, 1);
    mpz_fdiv_q(sig_star.sig_star, sig_star.sig_star, setup->n);
    mpz_mod(sig_star.sig_star, sig_star.sig_star, setup->n2);

    mpz_t phi_inv;
    mpz_init(phi_inv);
    mpz_invert(phi_inv, m_secret->phi_n, setup->n);

    mpz_mul(sig_star.sig_star, sig_star.sig_star, phi_inv);
    mpz_mod(sig_star.sig_star, sig_star.sig_star, setup->n);
    gmp_printf("decrypted e2: %Zd\n", sig_star.sig_star);

    mpz_t x;
    mpz_init(x);
    mpz_sub(x, sig_star.sig_star, m_secret->n_half);
    gmp_printf("x: %Zd\n", x);

    return sig_star;

    /* -----------------------------------------------------------------------*/
    //END MGR_STEP 5

}
