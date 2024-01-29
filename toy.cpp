#include "toy.h"
#include <iostream>
#include <cstdlib>
#include <cstring>

// Function to print binary representation of a number
void print_binary(int num, int num_bits) {
    for (int i = num_bits - 1; i >= 0; --i) {
        putchar((num & (1 << i)) ? '1' : '0');
    }
}

// Function to fill a buffer with small random values
static void modified_toy_fill_small(short* buffer, int n) {
    for (int k = 0; k < n; ++k) {
        short val = rand();
        val = (val >> 1 & 1) - (val & 1);
        if (val < 0)
            val += TK_Q;
        buffer[k] = val;
    }
}

// Polynomial multiplication in Z97[X]/(X^4+1)
static void modified_toy_polmul_naive(
    short* result,
    const short* poly1,
    const short* poly2,
    int add_to_result
) {
    // Implementation of polynomial multiplication
    result[0] = ((result[0] & -add_to_result) + poly1[0] * poly2[0] + NEGATE(poly1[3]) * poly2[1] + NEGATE(poly1[2]) * poly2[2] + NEGATE(poly1[1]) * poly2[3]) % TK_Q;
    result[1] = ((result[1] & -add_to_result) + poly1[1] * poly2[0] + poly1[0] * poly2[1] + NEGATE(poly1[3]) * poly2[2] + NEGATE(poly1[2]) * poly2[3]) % TK_Q;
    result[2] = ((result[2] & -add_to_result) + poly1[2] * poly2[0] + poly1[1] * poly2[1] + poly1[0] * poly2[2] + NEGATE(poly1[3]) * poly2[3]) % TK_Q;
    result[3] = ((result[3] & -add_to_result) + poly1[3] * poly2[0] + poly1[2] * poly2[1] + poly1[1] * poly2[2] + poly1[0] * poly2[3]) % TK_Q;
}

// Matrix-vector multiplication
static void modified_toy_mulmv(short* result, const short* mat, const short* vec) {
    // Initialize result matrix to zero
    memset(result, 0, TK_K * TK_N * sizeof(short));

    // Iterate through matrix and vector for multiplication
    for (int kv = 0, idx = 0; kv < TK_K * TK_N; kv += TK_N) {
        for (int k = 0; k < TK_K * TK_N; k += TK_N, idx += TK_N)
            modified_toy_polmul_naive(result + kv, mat + idx, vec + k, 1);
    }
}

// Matrix-vector multiplication with transposed matrix
static void modified_toy_mulmTv(short* result, const short* mat, const short* vec) {
    // Initialize result matrix to zero
    memset(result, 0, TK_K * TK_N * sizeof(short));

    // Iterate through matrix and vector for multiplication
    for (int kv = 0; kv < TK_K * TK_N; kv += TK_N) {
        for (int k = 0; k < TK_K * TK_N; k += TK_N)
            modified_toy_polmul_naive(result + kv, mat + TK_K * k + kv, vec + k, 1);
    }
}

// Dot product of two vectors
static void modified_toy_dot(short* result, const short* v1, const short* v2) {
    // Initialize result vector to zero
    memset(result, 0, TK_N * sizeof(short));

    // Iterate through vectors for dot product
    for (int k = 0; k < TK_K * TK_N; k += TK_N)
        modified_toy_polmul_naive(result, v1 + k, v2 + k, 1);
}

// Addition of two vectors
static void modified_toy_add(short* result, const short* v1, const short* v2, int count, int negate_v2) {
    for (int k = 0; k < count; ++k) {
        short val = v2[k];
        if (negate_v2)
            val = NEGATE(val);
        result[k] = (v1[k] + val) % TK_Q;
    }
}

// Main API function for key generation
void modified_toy_gen(short* public_key_A, short* public_key_t, short* secret_key) {
    short e[TK_K * TK_N];

    // Generate a random matrix for public_key_A
    for (int k = 0; k < TK_K * TK_K * TK_N; ++k)
        public_key_A[k] = rand() % TK_Q;

    // Generate small random values for secret_key and e
    modified_toy_fill_small(secret_key, TK_K * TK_N);
    modified_toy_fill_small(e, TK_K * TK_N);

    // Compute public_key_t = public_key_A . secret_key + e
    modified_toy_mulmv(public_key_t, public_key_A, secret_key);
    modified_toy_add(public_key_t, public_key_t, e, TK_K * TK_N, 0);
}

// Main API function for encryption
void modified_toy_enc(const short* public_key_A, const short* public_key_t, int plaintext, short* ciphertext_u, short* ciphertext_v) {
    short r[TK_K * TK_N], e1[TK_K * TK_N], e2[TK_N];

    // Generate small random values for r, e1, and e2
    modified_toy_fill_small(r, TK_K * TK_N);
    modified_toy_fill_small(e1, TK_K * TK_N);
    modified_toy_fill_small(e2, TK_N);

    // Compute ciphertext_u = A_transpose . r + e1
    modified_toy_mulmTv(ciphertext_u, public_key_A, r);
    modified_toy_add(ciphertext_u, ciphertext_u, e1, TK_K * TK_N, 0);

    // Compute ciphertext_v = t dot r + e2 + plaintext * q/2
    modified_toy_dot(ciphertext_v, public_key_t, r);
    modified_toy_add(ciphertext_v, ciphertext_v, e2, TK_N, 0);
    for (int k = 0; k < TK_N; ++k)
        ciphertext_v[k] = (ciphertext_v[k] + ((TK_Q >> 1) & -(plaintext >> (TK_N - 1 - k) & 1))) % TK_Q;
}

// Main API function for decryption
int modified_toy_dec(const short* secret_key, const short* ciphertext_u, const short* ciphertext_v) {
    short p[TK_N], plaintext;

    // Compute dot product of secret_key and ciphertext_u
    modified_toy_dot(p, secret_key, ciphertext_u);

    // Compute addition of ciphertext_v and dot product result
    modified_toy_add(p, ciphertext_v, p, TK_N, 1);

    plaintext = 0;

    // Reconstruct plaintext from the result
    for (int k = 0; k < TK_N; ++k) {
        int val = p[k];
        if (val > TK_Q / 2)
            val -= TK_Q;
        printf("%5d ", val);
        int bit = abs(val) > TK_Q / 4;
        plaintext |= bit << (TK_N - 1 - k);
    }

    return plaintext;
}
