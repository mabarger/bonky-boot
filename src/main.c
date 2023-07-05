#include <types.h>
#include <uart.h>

extern uint8_t sha256(uint8_t * data, uint32_t len, uint8_t * hash);

void __main(void) {
	int i = 0;

	print("\nHello from the best ISA\n");

	// TODO: Compute Hash over software

	// TODO: Verify signature from software

	uint8_t ret = sha256((uint8_t *)0, 0, (uint8_t *)0);

	while(true) {};
}
