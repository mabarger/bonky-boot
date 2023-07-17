#include <types.h>

#define NUMERI_MAX_BYTES 64

/* Big integer type */
typedef struct numeri {
	bool    used;
	uint8_t data[NUMERI_MAX_BYTES];
} numeri;

/* Initialize numeri cache */
bool numeri_init();

/* Allocate a numeri from the cache */
numeri *numeri_alloc();

/* Frees an allocated numeri */
void numeri_free(numeri *a);

/* Adds the numeris a and b, store the result in c */
void numeri_add(numeri *a, numeri *b, numeri *c);

/* Multiply the numeris a and b, store the result in c */
void numeri_mul(numeri *a, numeri *b, numeri *c);
