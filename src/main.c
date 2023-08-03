#include <numeri.h>
#include <memory.h>
#include <rsa.h>
#include <types.h>
#include <uart.h>

extern uint8_t sha256(uint8_t * data, uint32_t len, uint8_t * hash);

uint8_t signature[] = {
	100, 230, 171, 195, 247, 179, 139, 209, 122, 34, 18, 56, 82, 213, 34, 128,
	52, 223, 219, 142, 192, 65, 34, 252, 23, 205, 182, 225, 96, 141, 23, 7, 173,
	238, 103, 169, 15, 248, 43, 83, 193, 62, 31, 150, 134, 198, 83, 161, 227, 42,
	224, 59, 86, 48, 242, 2, 39, 219, 211, 124, 187, 7, 39, 233, 220, 174, 160,
	118, 221, 238, 111, 137, 211, 130, 175, 123, 126, 223, 252, 239, 97, 4, 106,
	246, 82, 30, 0, 77, 72, 43, 165, 128, 76, 63, 98, 87, 0, 36, 189, 51, 66, 180,
	145, 109, 211, 206, 80, 42, 205, 70, 50, 124, 126, 147, 29, 84, 53, 32,
	13, 125, 161, 44, 187, 204, 96, 151, 241, 90
};

void __main(void) {
	uint8_t hash[32];

	print("\nHello from the best ISA\n");

	/* Compute hash over software to boot */
	//sha256(XIPM_BASE, 8, hash);

	// TODO: Verify signature from software
	numeri_init();
	verify_signature(signature, hash);

	print("[~] Blocking Execution\n");
	while(true) {};
}
