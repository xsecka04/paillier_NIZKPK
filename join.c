#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gmp.h>
#include "paillier.h"
#include "join.h"

// e2 has real division
Setup_SGM* 
nizkpk_setup_SGM2(int param, paillier_prvkey_t* prv, paillier_pubkey_t* pub){

    Setup_SGM* res;
    rand_element* r;
    mpz_init(r->r);

    //mpz_init(res->g);
    paillier_keygen(param, &pub, &prv, paillier_get_rand_devurandom);

    //mpz_set(g, get_blinding_factor(pub, paillier_get_rand_devurandom));

    res->pub = *pub;
    //*(*res).g = *g;
    r = get_blinding_factor(pub, paillier_get_rand_devurandom);
    //*(*res).g = *r->r;
    //mpz_set(res->g, get_blinding_factor(pub, paillier_get_rand_devurandom));
    //mpz_clear(g);
    return res;

}

void 
nizkpk_setup_SGM(int param, Setup_SGM** setup, paillier_prvkey_t** prv, paillier_pubkey_t** pub){

    *setup = (Setup_SGM*) malloc(sizeof(Setup_SGM));
    paillier_keygen(param, pub, prv, paillier_get_rand_devurandom);
    (*setup)->pub = **pub;

    rand_element* r = get_blinding_factor(*pub, paillier_get_rand_devurandom);
    //mpz_mod(r->r, r->r, (*setup)->pub.n_squared);
    (*setup)->g = *r;

}



e1* generate_e1(Setup_SGM* setup, paillier_prvkey_t * prv){

    e1* res;
    res = (e1*) malloc(sizeof(e1));

    //paillier_plaintext_t* sk;

	  mpz_t r;
    //mpz_init(r);
    mpz_t x;
    //mpz_init(x);

    mpz_t two;
    mpz_init(two);
    mpz_set_ui(two, 2);

    mpz_t q;
    mpz_init(q);
    mpz_set_ui(q, 10000);


    mpz_t message;
    mpz_init(message);
    mpz_invert(message, two, (*setup).pub.n);
    mpz_mul(message, message, (*setup).pub.n);
    mpz_add(message, message, q);
    mpz_mod(message, message, prv->lambda);

    // Generate r
    gmp_randstate_t rand;
    init_rand(rand, paillier_get_rand_devurandom, (*setup).pub.bits / 8 + 1);
    do
      mpz_urandomb(r, rand, (*setup).pub.bits);
    while( mpz_cmp(r, (*setup).pub.n) >= 0 );
    mpz_mod(r, r, prv->lambda);

    //Calculate e1
    mpz_powm((*res).e1.c, (*setup).pub.n_plusone, message, (*setup).pub.n_squared);
    mpz_powm(x, r, (*setup).pub.n, (*setup).pub.n_squared);

    mpz_mul((*res).e1.c, (*res).e1.c, x);
    mpz_mod((*res).e1.c, (*res).e1.c, (*setup).pub.n_squared);

    mpz_clear(x);
    mpz_clear(r);
    gmp_randclear(rand);


    //*(*sk).m = *r->r;

    //*(*sk).m = **get_blinding_factor(&setup->pub, paillier_get_rand_devurandom);
    //mpz_init_set_ui(sk->m, get_blinding_factor(&setup->pub, paillier_get_rand_devurandom));

    //Encryption of the message
    //res->e1 = *nizkpk_paillier_enc(0, sk, setup);
    printf("ASD");
    //gmp_printf ("Parameter e1: %Zd\n", res->e1);

    return res;

}


paillier_ciphertext_t*
nizkpk_paillier_enc( paillier_ciphertext_t* res,
							paillier_plaintext_t* pt,
                            Setup_SGM* setup )
{
	mpz_t x;

	/* compute ciphertext */
	
	if( !res )
	{
		res = (paillier_ciphertext_t*) malloc(sizeof(paillier_ciphertext_t));
		mpz_init(res->c);
	}
  (*(*setup).pub.n_plusone);
	mpz_init(x);
	mpz_powm(res->c, (*setup).pub.n, pt->m, (*setup).pub.n_squared);
	mpz_powm(x, setup->g.r, (*setup).pub.n, (*setup).pub.n_squared);

	mpz_mul(res->c, res->c, x);
	mpz_mod(res->c, res->c, (*setup).pub.n_squared);

	mpz_clear(x);

	return res;
}

rand_element* get_blinding_factor(paillier_pubkey_t* pub, paillier_get_rand_t get_rand)
{

  rand_element* r;
  //r = (rand_element*) malloc(sizeof(rand_element));

	gmp_randstate_t rand;

	/* pick random blinding factor */

	//mpz_init(r->r);
 	init_rand(rand, get_rand, pub->bits / 8 + 1);
	do
		mpz_urandomb(r->r, rand, pub->bits);
	while( mpz_cmp(r->r, pub->n) >= 0 );

  gmp_randclear(rand);

  return r;

}

unsigned char * serialize_int(unsigned char *buffer, int value)
{
  /* Write big-endian int value into buffer; assumes 32-bit int and 8-bit char. */
  buffer[0] = value >> 24;
  buffer[1] = value >> 16;
  buffer[2] = value >> 8;
  buffer[3] = value;
  return buffer + 4;
}

unsigned char * serialize_char(unsigned char *buffer, char value)
{
  buffer[0] = value;
  return buffer + 1;
}

unsigned char * serialize_Setup_SGM(unsigned char *buffer, Setup_SGM* setup)
{
  buffer = paillier_pubkey_to_hex(&setup->pub);
  //buffer += "0000";
  //buffer += mpz_get_str(0, 16, &setup->g);
  return buffer;
}
