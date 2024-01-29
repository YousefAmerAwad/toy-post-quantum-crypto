#ifndef TOY_H_INCLUDED
#define TOY_H_INCLUDED

#define TK_K 3
#define TK_N 4
#define TK_Q 97
#define NEGATE(X) (TK_Q - (X))

void print_binary(int num, int num_bits);
void modified_toy_test();
int modified_toy_dec(const short* secret_key, const short* ciphertext_u, const short* ciphertext_v);
void modified_toy_enc(const short* public_key_A, const short* public_key_t, int plaintext, short* ciphertext_u, short* ciphertext_v);
void modified_toy_gen(short* public_key_A, short* public_key_t, short* secret_key);

static void modified_toy_add(short* result, const short* vector1, const short* vector2, int count, int negate_vector2);
static void modified_toy_dot(short* result, const short* vector1, const short* vector2);
static void modified_toy_mulmTv(short* result, const short* matrix, const short* vector);
static void modified_toy_mulmv(short* result, const short* matrix, const short* vector);
static void modified_toy_polmul_naive(short* result, const short* poly1, const short* poly2, int add_to_result);

#endif // TOY_H_INCLUDED



