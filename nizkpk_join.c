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

void primeFactors(mpz_t* phi_m, mpz_t* primes)
{
    mpz_t c;
    mpz_init(c);
    mpz_set_ui(c, 2);

    mpz_t c_prev;
    mpz_init(c_prev);

    mpz_t n;
    mpz_init(n);
    mpz_set(n, *phi_m);

    mpz_t i;
    mpz_init(i);

    int iter = 0;

    for(int i=0; i<10; i++){
        mpz_init_set_ui(primes[i], 1);
    }


    while( mpz_cmp_d(n, 1) > 0)
    {
        mpz_mod(i, n, c);
        
        if(mpz_cmp_d(i, 0) == 0){
            if(mpz_cmp(c, c_prev) != 0){
                mpz_set(c_prev, c);
                mpz_set(primes[iter], c);
                if (iter == 9){
                    break;
                }
                iter++;
            }
            mpz_divexact(n, n, c);
        }
        else mpz_add_ui(c, c, 1);
    }

    mpz_clear(c);
    mpz_clear(c_prev);
    mpz_clear(n);
    mpz_clear(i);

}

void generate_g(mpz_t* n, mpz_t* n2, mpz_t* phi, mpz_t* g) {


    mpz_t i;
    mpz_init(i);

    mpz_t k;
    mpz_init(k);

    do{

        generate_r_from_group(n, &k);
        mpz_powm(*g, k, *n, *n2);
        mpz_powm(i, *g, *phi, *n2);

    }while(mpz_cmp_ui(i, 1) != 0);
    //gmp_printf("G : %Zd\n", *g);
    //gmp_printf("G Assertion: %Zd\n", i);

    mpz_clear(i);
    mpz_clear(k);


}


void generate_g_bruteforce(mpz_t* n2, mpz_t* phi, mpz_t* ord, mpz_t* g) {
    
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

    mpz_t i;
    mpz_init(i);
    mpz_set_ui(i, 528);

    mpz_t j;
    mpz_init(j);

    mpz_t * factors;
    factors = malloc(sizeof(mpz_t) * 10);

    int isGen = 0;

    primeFactors(phi, factors);
    gmp_printf("Prime divisor of %Zd is: %Zd\n",*phi, factors[0]);
    gmp_printf("Prime divisor of %Zd is: %Zd\n",*phi, factors[1]);
    gmp_printf("Prime divisor of %Zd is: %Zd\n",*phi, factors[2]);
    gmp_printf("Prime divisor of %Zd is: %Zd\n",*phi, factors[3]);
    gmp_printf("Prime divisor of %Zd is: %Zd\n",*phi, factors[4]);

    do{
        mpz_urandomm(*g, rand, *n2);
        for(int i=0;i<10; i++){
            if(mpz_cmp_ui(factors[i], 1) != 0){
                mpz_divexact(j, *phi, factors[i]);
                mpz_powm(j, *g, j, *n2);
                gmp_printf("Testing %Zd ^ %Zd / %Zd mod %Zd = %Zd\n", *g, *phi, factors[i], *n2, j);

            }
            if (mpz_cmp_ui(j, 1) == 0){
                break;
            }

        }

        if ((mpz_cmp_ui(j, 1) != 0) && (mpz_cmp_ui(j, 0) != 0)){
            isGen = 1;
        }


    }while( (isGen == 0) || (mpz_cmp_ui(*g, 0) == 0));
    gmp_printf("Generated generator: %Zd\n",*g);

    
    mpz_powm(*g, *g, i, *n2);
    gmp_printf("Generated G: %Zd\n",*g);


    //do{
    //    mpz_urandomm(*g, rand, *n2);
    //    mpz_powm(j, *g, *ord, *n2);
    //}while( mpz_cmp(j, i) != 0 );
    //gmp_printf("Generated G using bruteforce: %Zd\n",*g);


    //mpz_urandomm(*g, rand, *n2);

    mpz_clear(i);
    mpz_clear(j);

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
    free(buf);

}

void generate_r_from_bitlenght(size_t length, mpz_t* r){
    
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

    mpz_urandomb(*r, rand, length);
    
    gmp_randclear(rand);
    mpz_clear(s);
    free(buf);


}

void generate_RSA(mpz_t* p, mpz_t* q, mpz_t* n, size_t size, int kappa){

    /**********RSA GENERATION***********************/

    int len = 512;

    mpz_t s;
	mpz_init(s);
    void* buf;
	buf = malloc(len);
	get_rand_seed(buf, len);
	mpz_import(s, size, 1, 1, 0, 0, buf);

    gmp_randstate_t rand;
    gmp_randinit_default(rand);
    gmp_randseed(rand, s);
    
    size *= 3*kappa;
    //printf("size of RSA: %zu\n", size);

	do
	{
		do
			mpz_urandomb(*p, rand, size/2 + 1);
		while( !mpz_probab_prime_p(*p, 10) );

		do
			mpz_urandomb(*q, rand, size/2 + 1);
		while( !mpz_probab_prime_p(*q, 10) );
    
        mpz_mul(*n, *p, *q);

	} while( !mpz_tstbit(*n, size - 1) );

    //gmp_printf("p_n: %Zd\n", p_n);
    //gmp_printf("q_n: %Zd\n", q_n);
    //gmp_printf("n: %Zd\n", setup->n);

    gmp_randclear(rand);
    mpz_clear(s);
    free(buf);
    
    /***********************************************/

}


void generate_nizkpk_setup(Setup_SGM* setup, Manager_S* m_secret, char* q_EC, char* g_EC, int kappa){
    
    //BEGIN MGR_SETUP
    /* -----------------------------------------------------------------------*/

    //EC params
    mpz_inits(setup->q_EC, setup->g_EC, NULL);
    //mpz_set_ui(setup->q_EC, q_EC);
    mpz_set_str(setup->q_EC, q_EC, 16);
    //mpz_set_ui(setup->g_EC, g_EC);
    mpz_set_str(setup->g_EC, g_EC, 16);



    //RSA params generation impl
    mpz_t p_n;
    mpz_init(p_n);
    //mpz_set_ui(p_n, p_RSA);
    mpz_t q_n;
    mpz_init(q_n);
    //mpz_set_ui(q_n, q_RSA);

    mpz_t q2;
    mpz_init(q2);
    mpz_pow_ui(q2, setup->q_EC, 2);

    size_t size = mpz_sizeinbase(q2, 2) + 1;
    //gmp_printf("q2: %Zd\n", q2);
    //printf("size of q2 in bits: %zu\n", size/2);
    
    mpz_init(setup->n);
    generate_RSA(&p_n, &q_n, &setup->n, size, kappa);

    mpz_inits(setup->n_goth, setup->h_goth, setup->g_goth, m_secret->phi_n_dash, NULL);

    mpz_t p_ng;
    mpz_init(p_ng);
    mpz_t q_ng;
    mpz_init(q_ng);

    mpz_t rand_goth;
    mpz_init(rand_goth);

    //Gothics
    generate_RSA(&p_ng, &q_ng, &setup->n_goth, size, kappa);
    mpz_add_ui(setup->h_goth, setup->n_goth, 1);
    //gmp_printf("n_goth: %Zd\n", &setup->n_goth);
    generate_r_from_group(&setup->n_goth, &rand_goth);

    mpz_powm(setup->g_goth, setup->h_goth, rand_goth, setup->n_goth);
    mpz_mul(m_secret->phi_n_dash, p_ng, q_ng);

    //Secrets impl
    mpz_init(m_secret->sk_m);

    generate_r_from_group(&setup->q_EC, &m_secret->sk_m);
    //mpz_set_ui(m_secret->sk_m, 40);

    //gmp_printf("SKm: %Zd\n", m_secret->sk_m);

    mpz_inits(setup->g, setup->h, setup->n2, m_secret->phi_n, NULL);

    mpz_add_ui(setup->h, setup->n, 1);

    mpz_pow_ui(setup->n2, setup->n, 2);
    mpz_sub_ui(p_n, p_n, 1);
    mpz_sub_ui(q_n, q_n, 1);
    mpz_mul(m_secret->phi_n, p_n, q_n);

    //gmp_printf("n2: %Zd\n",setup->n2);
    //gmp_printf("phi_n: %Zd\n", m_secret->phi_n);

    //TODO: Seems redundant
    mpz_init(m_secret->phi_n2);
    mpz_mul(m_secret->phi_n2, m_secret->phi_n, setup->n);

    mpz_t two;
    mpz_init(two);
    mpz_set_ui(two, 2);

    mpz_init(setup->n_half);
    mpz_invert(setup->n_half, two, m_secret->phi_n2);
    gmp_printf("n: %Zd\n", setup->n);
    gmp_printf("p: %Zd\n", p_n);
    gmp_printf("q: %Zd\n", q_n);

    gmp_printf("n_half: %Zd\n", setup->n_half);

    mpz_mul(setup->n_half, setup->n, setup->n_half);
    gmp_printf("n_half: %Zd\n", setup->n_half);

    mpz_mod(setup->n_half,setup->n_half, setup->n);
    //mpz_fdiv_q(setup->n_half, setup->n, two);
    gmp_printf("n_half: %Zd\n", setup->n_half);

    
    //G generation
    generate_g(&setup->n, &setup->n2, &m_secret->phi_n, &setup->g);
    //mpz_set_ui(setup->g, 157318);
    printf("size of n: %zu\n", mpz_sizeinbase(setup->n, 2));
    printf("size of g: %zu\n", mpz_sizeinbase(setup->g, 2));
    printf("size of h: %zu\n", mpz_sizeinbase(setup->h, 2));

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
    //gmp_printf("r: %Zd\n", m_secret->r);
    //mpz_set_ui(m_secret->r, 10);

    mpz_t e11;
    mpz_init(e11);

    //e1 calc
    mpz_init(e1.e1);
    mpz_add(e1.e1, setup->n_half, m_secret->sk_m);
    mpz_powm(e1.e1, setup->h, e1.e1, setup->n2);
    mpz_powm(e11, setup->g, m_secret->r, setup->n2);
    mpz_mul(e1.e1, e1.e1, e11);
    mpz_mod(e1.e1, e1.e1, setup->n2);
    //gmp_printf("e1: %Zd\n", e1.e1);

    mpz_inits(m_secret->r_dash, e1.c_goth, m_secret->r_dash, NULL);
    generate_r_from_group(&m_secret->phi_n_dash, &m_secret->r_dash);


    mpz_powm(e1.c_goth, setup->g_goth, m_secret->sk_m, setup->n_goth);
    mpz_powm(e11, setup->h_goth, m_secret->r_dash, setup->n_goth);
    mpz_mul(e1.c_goth, e1.c_goth, e11);
    mpz_mod(e1.c_goth, e1.c_goth, setup->n_goth);


    return e1;

    /* -----------------------------------------------------------------------*/
    //END MGR_STEP 1
}

E_2 generate_e2(Setup_SGM* setup, Sender_S* s_secret, E_1* e1, int kappa){

    //BEGIN SEND_STEP 3
    /* -----------------------------------------------------------------------*/
    E_2 e2;

    //Secrets impl
    mpz_inits(s_secret->sk_i, s_secret->r1, s_secret->r2, s_secret->r_bar, NULL);


    size_t r2_size = mpz_sizeinbase(setup->q_EC, 2)*kappa;
    size_t rbar_size = mpz_sizeinbase(setup->n_goth, 2)*kappa;


    generate_r_from_group(&setup->q_EC, &s_secret->sk_i);
    generate_r_from_group(&setup->q_EC, &s_secret->r1);
    generate_r_from_bitlenght(r2_size, &s_secret->r2);
    generate_r_from_bitlenght(r2_size, &s_secret->r_bar);
    generate_r_from_group(&setup->n, &s_secret->r_bar);


    //gmp_printf("S secrets: Ski=%Zd, r1=%Zd, r2=%Zd, rbar=%Zd \n", s_secret->sk_i, s_secret->r1, s_secret->r2, s_secret->r_bar);
    //mpz_set_ui(s_secret->r1, 13);
    //mpz_set_ui(s_secret->r2, 17);
    //mpz_set_ui(s_secret->r_bar, 11);
    //mpz_set_ui(s_secret->sk_i, 22);

    // e2 calc
    mpz_init(e2.e2);

    mpz_t e22, e23;
    mpz_inits(e22, e23, NULL);

    mpz_powm(e23, setup->h, setup->n_half, setup->n2);
    mpz_invert(e23, e23, setup->n2);
    mpz_mul(e23, e1->e1, e23);
    mpz_powm(e23, e23, s_secret->r1, setup->n2);

    //gmp_printf("e2 prelim 2 multiplied w mod n2: %Zd\n", e23);


    mpz_mul(e2.e2, s_secret->sk_i, s_secret->r1);
    mpz_mul(e22, s_secret->r2, setup->q_EC);
    mpz_add(e2.e2, e2.e2, e22);
    mpz_add(e2.e2, e2.e2, setup->n_half);
    mpz_powm(e2.e2, setup->h, e2.e2, setup->n2);

    mpz_powm(e22, setup->g, s_secret->r_bar, setup->n2);
    
    mpz_mul(e2.e2, e2.e2, e23);
    mpz_mul(e2.e2, e2.e2, e22);
    mpz_mod(e2.e2, e2.e2, setup->n2);
    //gmp_printf("e2: %Zd\n", e2.e2);


    mpz_inits(e2.c2_goth, NULL);
    mpz_powm(e2.c2_goth, setup->g_goth, s_secret->sk_i, setup->n_goth);
    mpz_powm(e22, setup->h_goth, s_secret->sk_i, setup->n_goth);
    mpz_mul(e2.c2_goth, e2.c2_goth, e22);
    mpz_mod(e2.c2_goth, e2.c2_goth, setup->n_goth);


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
    mpz_sub(sig_star.sig_star, sig_star.sig_star, setup->n_half);
    mpz_mod(sig_star.sig_star, sig_star.sig_star, setup->n);
    mpz_mod(sig_star.sig_star, sig_star.sig_star, setup->q_EC);
    //gmp_printf("decrypted x: %Zd\n", sig_star.sig_star);




    return sig_star;

    /* -----------------------------------------------------------------------*/
    //END MGR_STEP 5

}


int verify_sig(Sig_star* sig, Manager_S* m_secret, Sender_S* s_secret, Setup_SGM* setup){

    //BEGIN SEND_STEP 6
    /* -----------------------------------------------------------------------*/

    mpz_t test;
    mpz_init(test);

    mpz_add(test, s_secret->sk_i, m_secret->sk_m);
    mpz_mul(test, test, s_secret->r1);
    mpz_mod(test, test, setup->q_EC);
    //gmp_printf("Test: %Zd\n", test);

    if(mpz_cmp(sig->sig_star, test) == 0){

        mpz_clear(test);
        return 0;

    } else {

        mpz_clear(test);
        return 1;
    }

    /* -----------------------------------------------------------------------*/
    //END SEND_STEP 6

}

