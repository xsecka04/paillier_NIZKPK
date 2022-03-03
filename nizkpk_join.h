
typedef struct
{
    mpz_t q_EC;   
    mpz_t g_EC;   

	mpz_t n;
    mpz_t n2;
	mpz_t h;
    mpz_t g;

} Setup_SGM;


typedef struct
{
    mpz_t sk_m;

    mpz_t phi_n;
    mpz_t phi_n2;
    mpz_t n_half;

    mpz_t r;


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

} E_1;

typedef struct
{
    mpz_t e2;

} E_2;

typedef struct
{
    mpz_t sig_star;

} Sig_star;


void get_rand_seed( void* buf, int len);

void generate_g(mpz_t* n2, mpz_t* ord, mpz_t* g);

void generate_r_from_group(mpz_t* mod, mpz_t* r);

void generate_nizkpk_setup(Setup_SGM* setup, Manager_S* m_secret, char* q_EC, char* g_EC, int p_RSA, int q_RSA);

E_1 generate_e1(Setup_SGM* setup, Manager_S* m_secret);

E_2 generate_e2(Setup_SGM* setup, Sender_S* s_secret, E_1* e1);

Sig_star decrypt_e2(Setup_SGM* setup, Manager_S* m_secret, E_2* e2);
