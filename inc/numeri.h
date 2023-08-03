#include <types.h>

#define NUMERI_MAX_BYTES 256

/* Big integer type */
typedef struct numeri {
	bool    used;
	uint8_t data[NUMERI_MAX_BYTES];
} numeri;

/* Returns the size of the numeri in bits */
uint32_t numeri_size(numeri *a);

/* Returns true if a is bigger than b, otherwise false */
bool numeri_is_bigger(numeri *a, numeri *b);

/* Initialize numeri cache */
bool numeri_init();

/* Allocate a numeri from the cache */
numeri *numeri_alloc();

/* Copies n bytes of given integer data at ptr into the numeri a */
void numeri_load(numeri *a, void *ptr, size_t n);

/* Sets the given numeri to the value of the given integer */
void numeri_set(numeri *a, int b);

/* Copies numeri a into numberi b (b = a) */
void numeri_copy(numeri *a, numeri *b);

/* Frees an allocated numeri */
void numeri_free(numeri *a);

/* Resets a numeri to 0 */
void numeri_clean(numeri *a);

/* Adds the numeris a and b, store the result in c */
void numeri_add(numeri *a, numeri *b, numeri *c);

/* Substracts numeri b from numeri a and stores the result in c (c = a-b) */
/* Returns false if the result would be negative, otherwise true */
bool numeri_sub(numeri *a, numeri *b, numeri *c);

/* Multiply the numeris a and b, store the result in c */
void numeri_mul(numeri *a, numeri *b, numeri *c);

/* Computes b = a % n */
void numeri_mod(numeri *a, numeri *b, numeri *n);

/* Computes pow(a, b) % n */
void numeri_pow(numeri *a, uint32_t b, numeri *c, numeri *n);
