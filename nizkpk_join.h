
typedef struct
{
    mpz_t q_EC;   
    mpz_t g_EC;   

	mpz_t n;
    mpz_t n2;
    mpz_t n_half;
	mpz_t h;
    mpz_t g;

	mpz_t n_goth;
	mpz_t g_goth;
	mpz_t h_goth;

    //mpz_t pk_s;


} Setup_SGM;


typedef struct
{
    mpz_t sk_m;

    mpz_t phi_n;
    mpz_t phi_n_dash;
    mpz_t phi_n2; //Redundant

    mpz_t r;
    mpz_t r_dash;


} Manager_S;

typedef struct
{
    mpz_t sk_i;

    mpz_t r1;
    mpz_t r2;
    mpz_t r_bar;

} Sender_S;


typedef struct
{
    mpz_t e1;

    mpz_t c_goth;


} E_1;

typedef struct
{
    mpz_t e2;

    mpz_t c2_goth;
    //mpz_t pk_i;


} E_2;

typedef struct
{
    mpz_t sig_star;

} Sig_star;

void primeFactors(mpz_t* phi_m, mpz_t* primes);

void get_rand_seed( void* buf, int len);

void generate_g(mpz_t* n2, mpz_t* phi, mpz_t* ord, mpz_t* g);

void generate_r_from_group(mpz_t* mod, mpz_t* r);

void generate_nizkpk_setup(Setup_SGM* setup, Manager_S* m_secret, char* q_EC, char* g_EC, int kappa);

E_1 generate_e1(Setup_SGM* setup, Manager_S* m_secret);

E_2 generate_e2(Setup_SGM* setup, Sender_S* s_secret, E_1* e1, int kappa);

Sig_star decrypt_e2(Setup_SGM* setup, Manager_S* m_secret, E_2* e2);

int verify_sig(Sig_star* sig, Manager_S* m_secret, Sender_S* s_secret, Setup_SGM* setup);
