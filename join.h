
/*
	Include gmp.h before including this file.
*/


typedef struct
{
	paillier_ciphertext_t e1;  /* managers messge e1 */
	mpz_t n;   /* commitment */
} e1;

typedef struct
{
	paillier_pubkey_t pub;   /* public key (h_b, n_b)*/
	mpz_t g; /* blinding factor (g_b) */
	/* TODO: (g2, q) */
} Setup_SGM;


typedef struct
{
	mpz_t r;   /* randomly selected element */
} rand_element;



Setup_SGM* nizkpk_setup_SGM(int param, paillier_prvkey_t* prv, paillier_pubkey_t* pub);

paillier_ciphertext_t*
nizkpk_paillier_enc( paillier_ciphertext_t* res,
							paillier_plaintext_t* pt,
                            Setup_SGM* setup );

rand_element* 
get_blinding_factor(paillier_pubkey_t* pub, paillier_get_rand_t get_rand);

e1* generate_e1(Setup_SGM* setup);