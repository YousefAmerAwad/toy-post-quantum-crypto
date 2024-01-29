#include <iostream>
#include "toy.h"

int main() {
    // Key generation: public_key_A and public_key_t are public keys, secret_key is private key
    short public_key_A[TK_K * TK_K * TK_N], public_key_t[TK_K * TK_N];
    short secret_key[TK_K * TK_N];

    modified_toy_gen(public_key_A, public_key_t, secret_key);

    // Encryption and Decryption for a set of messages
    for (int plaintext = 0; plaintext < 16; ++plaintext) {
        short ciphertext_u[TK_K * TK_N], ciphertext_v[TK_N];

        // Encrypt message using public keys
        modified_toy_enc(public_key_A, public_key_t, plaintext, ciphertext_u, ciphertext_v);

        // Decrypt ciphertext using private key
        short decrypted_plaintext = modified_toy_dec(secret_key, ciphertext_u, ciphertext_v);

        // Display results: original message, decrypted message, and their binary representations
        printf("%3d %3d ", plaintext, decrypted_plaintext);
        print_binary(plaintext, TK_N);
        printf(" ");
        print_binary(decrypted_plaintext, TK_N);
        printf(" ");
        print_binary(plaintext ^ decrypted_plaintext, TK_N);
        printf("\n");
    }

    return 0;
}
