#include <types.h>
#include <numeri.h>

bool verify_signature(uint8_t *signature, uint8_t *hash) {
	numeri *a = numeri_alloc();
	numeri *b = numeri_alloc();
	numeri *c = numeri_alloc();

	numeri_mul(a, b, c);
	return true;
}
