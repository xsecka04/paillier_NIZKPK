#include <stdio.h>
#include <gmp.h>
#include "paillier.h"

void generate_keys(int *buff) {
    paillier_pubkey_t* pub;
    paillier_prvkey_t* priv;
    int len = 1000;
    paillier_keygen(len, &pub, &priv, paillier_get_rand_devurandom);
    buff[0] = priv->lambda;
    buff[1] = pub->n;
}

int main() {
    paillier_pubkey_t* pub;
    paillier_prvkey_t* priv;
    int len = 1000;
    paillier_keygen(len, &pub, &priv, paillier_get_rand_devurandom);
    printf("Private key (lambda): %d\n", priv->lambda);
    printf("Public key (n): %d\n", pub->n);
    char *message = "aasdaasdasd";
    paillier_ciphertext_t *enc = paillier_enc(0, pub, paillier_plaintext_from_str(message), paillier_get_rand_devurandom);
    printf("Ciphertext: %d\n", enc->c);
    paillier_plaintext_t *dec = paillier_dec(0, pub, priv, enc);
    char *m = paillier_plaintext_to_str(dec);
    printf("Plaintext: %s\n", m);
    return 0;
}