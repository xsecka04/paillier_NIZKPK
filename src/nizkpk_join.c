#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gmp.h>
#include "../lib/nizkpk_join.h"
#include <openssl/bn.h>

#define kappa 3

const char* SETUP_JSON_OUT = "{\n\t\"n\": \"%s\",\n\t\"g\": \"%s\",\n\t\"h\": \"%s\",\n\t\"n_goth\": \"%s\",\n\t\"g_goth\": \"%s\",\n\t\"h_goth\": \"%s\",\n\t\"q_EC\": \"%s\"\n}";
const char* E1_JSON_OUT = "{\n\t\"e_1\": \"%s\",\n\t\"c_goth\": \"%s\"\n}";
const char* E2_JSON_OUT = "{\n\t\"e_2\": \"%s\",\n\t\"c2_goth\": \"%s\"\n}";
const char* SIG_STAR_JSON_OUT = "{\n\t\"sig_star\": \"%s\"\n}";



const char* SETUP_JSON_IN = "{\n\t\"n\": \"%[^\"]\",\n\t\"g\": \"%[^\"]\",\n\t\"h\": \"%[^\"]\",\n\t\"n_goth\": \"%[^\"]\",\n\t\"g_goth\": \"%[^\"]\",\n\t\"h_goth\": \"%[^\"]\",\n\t\"q_EC\": \"%[^\"]\"\n}";
const char* E1_JSON_IN = "{\n\t\"e_1\": \"%[^\"]\",\n\t\"c_goth\": \"%[^\"]\"\n}";
const char* E2_JSON_IN = "{\n\t\"e_2\": \"%[^\"]\",\n\t\"c2_goth\": \"%[^\"]\"\n}";
const char* SIG_STAR_JSON_IN = "{\n\t\"sig_star\": \"%[^\"]\"\n}";


void get_rand_seed(void* buf, int len)
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

    mpz_t* factors;
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

void generate_RSA(mpz_t* p, mpz_t* q, mpz_t* n, size_t size){

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
    printf("size of RSA: %zu\n", size);

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
    
}

void generate_RSA_SSL(mpz_t* p, mpz_t* q, mpz_t* n, size_t size){

    BIGNUM *p_big = BN_new();
    BIGNUM *q_big = BN_new();
    BIGNUM *n_big = BN_new();

    size *= 3*kappa;

    clock_t start, end;
    double cpu_time_used;

    BN_CTX *ctx = BN_CTX_secure_new();
    BN_generate_prime_ex2(p_big, size/2+1, 0, NULL, NULL, NULL, ctx);
    BN_generate_prime_ex2(q_big, size/2+1, 0, NULL, NULL, NULL, ctx);

    char* p_hex = BN_bn2hex(p_big);
    char* q_hex = BN_bn2hex(q_big);

    mpz_set_str(*p, p_hex, 16);
    mpz_set_str(*q, q_hex, 16);

    mpz_mul(*n, *p, *q);

    BN_free(p_big);
    BN_free(q_big);
    BN_free(n_big);
    BN_CTX_free(ctx);
    OPENSSL_free(p_hex);
    OPENSSL_free(q_hex);

}

void generate_nizkpk_setup(Setup_SGM* setup, Manager_S* m_secret, char* q_EC){
    
    //EC params
    mpz_inits(setup->q_EC, NULL);
    //mpz_set_ui(setup->q_EC, q_EC);
    mpz_set_str(setup->q_EC, q_EC, 16);



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
    generate_RSA_SSL(&p_n, &q_n, &setup->n, size);

    mpz_inits(setup->n_goth, setup->h_goth, setup->g_goth, m_secret->phi_n_goth, NULL);

    mpz_t p_ng;
    mpz_init(p_ng);
    mpz_t q_ng;
    mpz_init(q_ng);

    mpz_t rand_goth;
    mpz_init(rand_goth);

    //Gothics
    generate_RSA_SSL(&p_ng, &q_ng, &setup->n_goth, size);
    //mpz_add_ui(setup->h_goth, setup->n_goth, 1);
    //gmp_printf("n_goth: %Zd\n", &setup->n_goth);
    generate_r_from_group(&setup->n_goth, &setup->h_goth);

    mpz_sub_ui(p_ng, p_ng, 1);
    mpz_sub_ui(q_ng, q_ng, 1);
    mpz_mul(m_secret->phi_n_goth, p_ng, q_ng);

    generate_r_from_group(&m_secret->phi_n_goth, &rand_goth);

    mpz_powm(setup->g_goth, setup->h_goth, rand_goth, setup->n_goth);



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

    mpz_t two;
    mpz_init(two);
    mpz_set_ui(two, 2);

    mpz_init(setup->n_half);
    //gmp_printf("n: %Zd\n", setup->n);
    //gmp_printf("p: %Zd\n", p_n);
    //gmp_printf("q: %Zd\n", q_n);

    mpz_fdiv_q(setup->n_half, setup->n, two);
    //gmp_printf("n_half: %Zd\n", setup->n_half);

    
    //G generation
    generate_g(&setup->n, &setup->n2, &m_secret->phi_n, &setup->g);
    //mpz_set_ui(setup->g, 157318);
    printf("size of n: %zu\n", mpz_sizeinbase(setup->n, 2));
    printf("size of g: %zu\n", mpz_sizeinbase(setup->g, 2));
    printf("size of h: %zu\n", mpz_sizeinbase(setup->h, 2));

    mpz_clears(two, p_n, q_n, p_ng, q_ng, q2, rand_goth, NULL);

}

E_1 generate_e1(Setup_SGM* setup, Manager_S* m_secret){

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
    generate_r_from_group(&m_secret->phi_n_goth, &m_secret->r_dash);


    mpz_powm(e1.c_goth, setup->g_goth, m_secret->sk_m, setup->n_goth);
    mpz_powm(e11, setup->h_goth, m_secret->r_dash, setup->n_goth);
    mpz_mul(e1.c_goth, e1.c_goth, e11);
    mpz_mod(e1.c_goth, e1.c_goth, setup->n_goth);

    mpz_clear(e11);

    return e1;

}

E_2 generate_e2(Setup_SGM* setup, Sender_S* s_secret, E_1* e1){

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

}

E_2 generate_e2_parallel(Setup_SGM* setup, Sender_S* s_secret, E_1* e1){

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

}


Sig_star decrypt_e2(Setup_SGM* setup, Manager_S* m_secret, E_2* e2){


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

    mpz_clear(phi_inv);




    return sig_star;

}


int verify_sig(Sig_star* sig, Manager_S* m_secret, Sender_S* s_secret, Setup_SGM* setup){


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

}

int JSON_serialize_Setup_par(Setup_SGM* setup){

    FILE* fp;
    fp = fopen("par.json", "w+");

    if(fp == NULL){
        return 1;
    }

    fprintf(fp, SETUP_JSON_OUT, mpz_get_str(NULL, 16, setup->n), mpz_get_str(NULL, 16, setup->g), 
            mpz_get_str(NULL, 16, setup->h), mpz_get_str(NULL, 16, setup->n_goth), mpz_get_str(NULL, 16, setup->g_goth), 
            mpz_get_str(NULL, 16, setup->h_goth), mpz_get_str(NULL, 16, setup->q_EC));
    
    fclose(fp);

    return 0;
}

int JSON_serialize_e1(E_1* e1){

    FILE* fp;
    fp = fopen("e1.json", "w+");

    if(fp == NULL){
        return 1;
    }

    fprintf(fp, E1_JSON_OUT, mpz_get_str(NULL, 16, e1->e1), mpz_get_str(NULL, 16, e1->c_goth));
    
    fclose(fp);
    
    return 0;
}

int JSON_serialize_e2(E_2* e2){

    FILE* fp;
    fp = fopen("e2.json", "w+");

    if(fp == NULL){
        return 1;
    }

    fprintf(fp, E2_JSON_OUT, mpz_get_str(NULL, 16, e2->e2), mpz_get_str(NULL, 16, e2->c2_goth));
    
    fclose(fp);
    return 0;
}

int JSON_serialize_sig_star(Sig_star* sig_star){

    FILE* fp;
    fp = fopen("sig_star.json", "w+");

    if(fp == NULL){
        return 1;
    }

    fprintf(fp, SIG_STAR_JSON_OUT, mpz_get_str(NULL, 16, sig_star->sig_star));
    
    fclose(fp);
    return 0;
}


int JSON_deserialize_Setup_par(Setup_SGM* setup){

    char n[6000];
    char g[10000];
    char h[6000];
    char n_g[6000];
    char h_g[6000];
    char g_g[6000];
    char q_EC[500];

    FILE* fp;
    fp = fopen("par.json", "r");

    if(fp == NULL){
        return 1;
    }

    fscanf(fp, SETUP_JSON_IN, n, g, h, n_g, h_g, g_g, q_EC);

    mpz_init_set_str(setup->n, n, 16);
    mpz_init_set_str(setup->g, g, 16);
    mpz_init_set_str(setup->h, h, 16);
    mpz_init_set_str(setup->n_goth, n_g, 16);
    mpz_init_set_str(setup->g_goth, h_g, 16);
    mpz_init_set_str(setup->h_goth, g_g, 16);
    mpz_init_set_str(setup->q_EC, q_EC, 16);

    mpz_init(setup->n2);
    mpz_mul(setup->n2, setup->n, setup->n);
    
    mpz_t two;
    mpz_init(two);
    mpz_set_ui(two, 2);

    mpz_init(setup->n_half);
    mpz_fdiv_q(setup->n_half, setup->n, two);

    fclose(fp);
    return 0;

}


int JSON_deserialize_e1(E_1* e1){

    char e1c[6000];
    char c_g[6000];
    FILE* fp;
    fp = fopen("e1.json", "r");

    if(fp == NULL){
        return 1;
    }


    fscanf(fp, E1_JSON_IN, e1c, c_g);

    mpz_init_set_str(e1->e1, e1c, 16);
    mpz_init_set_str(e1->c_goth, c_g, 16);

    fclose(fp);
    return 0;

}

int JSON_deserialize_e2(E_2* e2){

    char e2c[6000];
    char c2_g[6000];
    FILE* fp;
    fp = fopen("e2.json", "r");

    if(fp == NULL){
        return 1;
    }


    fscanf(fp, E2_JSON_IN, e2c, c2_g);

    mpz_init_set_str(e2->e2, e2c, 16);
    mpz_init_set_str(e2->c2_goth, c2_g, 16);

    fclose(fp);
    return 0;

}

int JSON_deserialize_sig_star(Sig_star* sig_star){

    char sig[6000];
    FILE* fp;
    fp = fopen("sig_star.json", "r");

    if(fp == NULL){
        return 1;
    }


    fscanf(fp, SIG_STAR_JSON_IN, sig);

    mpz_init_set_str(sig_star->sig_star, sig, 16);

    fclose(fp);
    return 0;

}



