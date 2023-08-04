#include <types.h>
#include <numeri.h>

bool verify_signature(uint8_t *signature, uint8_t *hash) {
	numeri *a = numeri_alloc();
	numeri *b = numeri_alloc();
	numeri *c = numeri_alloc();
	numeri *d = numeri_alloc();

	numeri_load(a, signature, 128);
	numeri_load(b, signature, 128);

	numeri_mul(a, b, c);
	numeri_set(b, 0xf27108);
	numeri_mod(c, b, a);
	return true;
}
