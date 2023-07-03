#include <uart.h>

#define UART_THR	UART_BASE + 0x00
#define UART_RBR	UART_BASE + 0x00
#define UART_LSR	UART_BASE + 0x05

#define UART_SR_RXRDY (0x1u << 0)
#define UART_SR_TXRDY (0x1u << 1)

void print(char *string) {
	while(*string != 0x00) {
		cputchar((int) *string);
		string++;
	}
}

void cputchar(int c) {
	uint8_t *uart_lsr = (uint8_t *) UART_LSR;
	uint8_t *uart_thr = (uint8_t *) UART_THR;

	// Wait for UART to be ready
	while (*uart_lsr & UART_SR_RXRDY) {};

	// Write character
	*uart_thr = (uint8_t) c;

	return;
}
