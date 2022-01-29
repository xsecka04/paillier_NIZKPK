#include <stdio.h>
#include <gmp.h>
#include "paillier.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#define PORT 8080
#define BUFFER_LENGTH 1000


void receive_msg()
{
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        exit(EXIT_FAILURE);
    }
   
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
       
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 
    {
        printf("\nInvalid address/ Address not supported \n");
        exit(EXIT_FAILURE);
    }
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        exit(EXIT_FAILURE);
    }
    send(sock , hello , strlen(hello) , 0 );
    printf("Hello message sent\n");
    valread = read( sock , buffer, 2048);

    paillier_ciphertext_t *foo = paillier_ciphertext_from_bytes(buffer, BUFFER_LENGTH);
    gmp_printf ("Received ciphertext: %Zd\n", foo->c);
}

int main() {

    //Generating keypair
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

    receive_msg();
    return 0;
}
