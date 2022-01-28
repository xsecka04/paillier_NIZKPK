#include <stdio.h>
#include <gmp.h>
#include "paillier.h"


int main() {

    //Generating keypairs
    paillier_pubkey_t* pub;
    paillier_prvkey_t* priv;
    int len = 100;
    paillier_keygen(len, &pub, &priv, paillier_get_rand_devurandom);
    gmp_printf ("Private key (lambda): %Zd\n", priv->lambda);
    gmp_printf ("Public key (n): %Zd\n", pub->n);

    char *message = "BigSecret";

    //Encryption of the message
    paillier_ciphertext_t *enc = paillier_enc(0, pub, paillier_plaintext_from_str(message), paillier_get_rand_devurandom);
    gmp_printf ("Ciphertext: %Zd\n", enc->c);

    //Decryption of the ciphertext
    paillier_plaintext_t *dec = paillier_dec(0, pub, priv, enc);
    char *m = paillier_plaintext_to_str(dec);
    printf("Plaintext: %s\n", m);

    return 0;
}