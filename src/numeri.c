#include <memory.h>
#include <numeri.h>
#include <util.h>

/* Cache that contains the available big integers */
/* The numeri cache takes up the middle 8KB of the DTIM */
static numeri *numeri_cache = (numeri *) DTIM_BASE + (DTIM_SIZE/4);
static const size_t numeri_cnt = 16;

uint32_t numeri_size(numeri *a) {
	uint32_t curr_byte = NUMERI_MAX_BYTES-1;
	while (curr_byte) {
		if (a->data[curr_byte] != 0x00) {
			break;
		}
		curr_byte--;
	}

	// Check the highest bit that is set
	uint32_t curr_bit = 0;
	while((a->data[curr_byte] >> curr_bit)) {
		curr_bit++;
	}
	return curr_byte * 8 + curr_bit;
}

bool numeri_is_bigger(numeri *a, numeri *b) {
	uint32_t size_a = numeri_size(a);
	uint32_t size_b = numeri_size(b);

	// If the size in bits is different we don't need to look any further
	if (size_a != size_b) {
		return size_a > size_b;
	}

	// Both numeris have the same amount of bits, so we'll just check the byte values
	int32_t curr_byte = (size_a / 8) + 1;
	uint32_t *awp = (uint32_t *)&a->data;
	uint32_t *bwp = (uint32_t *)&b->data;
	while (curr_byte >= 0) {
		if (awp[curr_byte] != bwp[curr_byte]) {
			return awp[curr_byte] > bwp[curr_byte];
		}
		curr_byte--;
	}
}

// Aka strcmp, returns 0 if they are equal, -1 if a is smaller than b, 1 if a is bigger than b
int numeri_cmp(numeri *a, numeri *b) {
	size_t curr_byte = NUMERI_MAX_BYTES - 1;

	while (curr_byte) {
		if(a->data[curr_byte] != b->data[curr_byte]) {
			return a->data[curr_byte] > b->data[curr_byte] ? 1 : -1;
		}
		curr_byte--;
	}

	return 0;
}

bool numeri_init() {
	// Initialize the numeri cache
	numeri *curr_numeri = numeri_cache;
	for (size_t numeri_id = 0; numeri_id < numeri_cnt; numeri_id++) {
		curr_numeri->used = false;
		curr_numeri++;
	}
}

numeri *numeri_alloc() {
	// Look for an available numeri and return it
	numeri *curr_numeri = numeri_cache;
	for (size_t numeri_id = 0; numeri_id < numeri_cnt; numeri_id++) {
		if (curr_numeri->used == false) {
			curr_numeri->used = true;
			return curr_numeri;
		}
		curr_numeri++;
	}
	return NULL;
}

void numeri_load(numeri *a, void *ptr, size_t n) {
	uint32_t *cnp = (uint32_t *)&a->data;
	uint32_t *csp = (uint32_t *)ptr;
	for (size_t word_idx = 0; word_idx < n/sizeof(uint32_t); word_idx++) {
		cnp[word_idx] = csp[word_idx];
	}
}

void numeri_set(numeri *a, int b) {
	numeri_clean(a);
	*((uint32_t *)&a->data) = b;
}

void numeri_copy(numeri *a, numeri *c) {
	uint32_t *awp = (uint32_t *)&a->data;
	uint32_t *cwp = (uint32_t *)&c->data;
	for (size_t word_idx = 0; word_idx < NUMERI_MAX_BYTES/sizeof(uint32_t); word_idx++) {
		cwp[word_idx] = awp[word_idx];
	}
}

void numeri_free(numeri *a) {
	a->used = false;
}

void numeri_clean(numeri *a) {
	uint32_t *curr_word = (uint32_t *)a->data;

	for (size_t i = 0; i < NUMERI_MAX_BYTES/sizeof(uint32_t); i++) {
		*curr_word = 0x00000000;
		curr_word++;
	}
}

void numeri_add(numeri *a, numeri *b, numeri *c) {
	// Add numeris byte by byte
	uint16_t temp = 0;
	uint16_t carry = 0;
	
	for (size_t byte_idx = 0; byte_idx < NUMERI_MAX_BYTES; byte_idx++) {
		temp = (uint16_t) (a->data[byte_idx] + b->data[byte_idx] + carry);
		carry = temp >> 8;
		c->data[byte_idx] = (temp & 0xFF);
	}
}

void numeri_or(numeri *a, numeri *b, numeri *c) {
	// OR numeris word by word
	uint16_t temp = 0;
	
	uint32_t *awp = (uint32_t *)&a->data;
	uint32_t *bwp = (uint32_t *)&b->data;
	uint32_t *cwp = (uint32_t *)&c->data;
	for (size_t word_idx = 0; word_idx < NUMERI_MAX_BYTES/sizeof(uint32_t); word_idx++) {
		cwp[word_idx] = awp[word_idx] | bwp[word_idx];
	}
}

bool numeri_sub(numeri *a, numeri *b, numeri *c) {
	// Substract numeris byte by byte
	uint16_t a_inv = 0;
	uint16_t b_bor = 0;
	uint16_t borrow = 0;

	for (size_t byte_idx = 0; byte_idx < NUMERI_MAX_BYTES; byte_idx++) {
		a_inv = (uint16_t) (a->data[byte_idx] + 0xFF + 1);
		b_bor = (uint16_t) (b->data[byte_idx] + borrow);
		c->data[byte_idx] = ((a_inv - b_bor) & 0xFF);
		borrow = ((a_inv - b_bor) <= 0xFF);
	}

	return true;
}

void numeri_mul(numeri *a, numeri *b, numeri *c) {
	// Zero result to avoid interference
	numeri_clean(c);

	// Multiply numeris byte by byte karatsuba-style
	numeri *row = numeri_alloc();
	numeri *temp = numeri_alloc();

	for (size_t a_idx = 0; a_idx < NUMERI_MAX_BYTES; a_idx++) {
		numeri_clean(row);
		
		for (size_t b_idx = 0; b_idx < NUMERI_MAX_BYTES; b_idx++) {
			if (a_idx + b_idx < NUMERI_MAX_BYTES) {
				// Compute temporary result and add via column/row coords to whole result
				numeri_clean(temp);
				uint16_t col = ((uint16_t) a->data[a_idx]) * ((uint16_t) b->data[b_idx]);
				*((uint16_t *)&temp->data[a_idx+b_idx]) = col;

				numeri_add(temp, row, row);
			}
		}
		numeri_add(c, row, c);
	}

	// Free temporary numeris, off you go into the sunset
	numeri_free(row);
	numeri_free(temp);
}

static void numeri_lshift_1(numeri *ac) {
	for (size_t byte_idx = (NUMERI_MAX_BYTES - 1); byte_idx > 0; byte_idx--) {
		ac->data[byte_idx] = (ac->data[byte_idx] << 1) | (ac->data[byte_idx - 1] >> 7);
	}
	ac->data[0] <<= 1;
}

static void numeri_rshift_1(numeri *ac) {
	for (size_t byte_idx = 0; byte_idx < (NUMERI_MAX_BYTES - 1); byte_idx++) {
		ac->data[byte_idx] = (ac->data[byte_idx] >> 1) | (ac->data[byte_idx + 1] << 7);
	}
	ac->data[NUMERI_MAX_BYTES-1] >>= 1;
}

void numeri_div(numeri *a, numeri *b, numeri *c) {
	// Zero result to avoid interference
	numeri_clean(c);

	// Long division
	numeri *mulp = numeri_alloc();
	numeri *denom = numeri_alloc();
	numeri *temp = numeri_alloc();
	numeri_set(mulp, 1);
	numeri_copy(b, denom);
	numeri_copy(a, temp);

	bool overflow = false;
	while (numeri_is_bigger(a, denom)) {
		// Approximate denominator
		if (denom->data[NUMERI_MAX_BYTES-1] >= 0x80) {
			overflow = true;
			break;
		}

		numeri_lshift_1(mulp);
		numeri_lshift_1(denom);
	}

	if (overflow == false) {
		numeri_rshift_1(mulp);
		numeri_rshift_1(denom);
	}

	// Apply denominator to find divisor
	while (numeri_size(mulp) != 0) {
		if (numeri_cmp(temp, denom) >= 0) {
			numeri_sub(temp, denom, temp);
			numeri_or(c, mulp, c);
		}

		numeri_rshift_1(mulp);
		numeri_rshift_1(denom);
	}

	// Free temporary numeris, off you go into the sunset
	numeri_free(mulp);
	numeri_free(denom);
	numeri_free(temp);
}

/* c = a^b */
/* Neither fast, nor accurate, but provides a reasonable result in a reasonable time */
static void numeri_pow_slow(numeri *a, uint32_t b, numeri *c) {
	numeri *temp = numeri_alloc();
	size_t limit = int_sqrt(b);

	numeri_copy(a, c);
	for (size_t i = 0; i < limit; i++) {
		numeri_mul(c, c, temp);
		numeri_copy(temp, c);
	}

	// Free temporary numeri, off you go into the sunset
	numeri_free(temp);
}

/* Shifts a numeri left such that it reaches a certain length in bytes */
static void numeri_shift_to_length(numeri *a, uint32_t len, numeri *c) {
	uint32_t size_diff = (len - (numeri_size(a) / 8));

	for (size_t byte_idx = size_diff; byte_idx < NUMERI_MAX_BYTES; byte_idx++) {
		c->data[byte_idx] = a->data[byte_idx - size_diff];
	}
}

void numeri_mod(numeri *a, numeri *n, numeri *c) {
	// Zero result to avoid interference
	numeri_clean(c);

	if (numeri_cmp(a, n) == -1) {
		numeri_copy(a, c);
		return;
	}

	// This is the Shortening of the Way
	numeri *divisor = numeri_alloc();
	numeri *approx = numeri_alloc();

	numeri_div(a, n, divisor);
	numeri_mul(divisor, n, approx);
	numeri_sub(a, approx, c);

	// Free temporary numeris, off you go into the sunset
	numeri_free(divisor);
	numeri_free(approx);
}

void numeri_pow(numeri *a, uint32_t b, numeri *n, numeri *c) {
	// Zero result to avoid interference
	numeri_clean(c);

	uint32_t curr_e = 1;
	uint32_t npot_e = b-1;

	numeri *square = numeri_alloc();
	numeri *temp = numeri_alloc();

	// Square and Multiply approximation
	numeri_copy(a, temp);
	while (curr_e != npot_e) {
		numeri_mul(temp, temp, square);
		numeri_mod(square, n, temp);
		curr_e *= 2;
	}

	// Transfer result and do final mul/mod
	numeri_mul(temp, a, square);
	numeri_mod(square, n, c);

	// Free temporary numeris, off you go into the sunset
	numeri_free(temp);
	numeri_free(square);
}
