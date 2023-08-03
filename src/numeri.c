#include <memory.h>
#include <numeri.h>

/* Cache that contains the available big numbers*/
static numeri *numeri_cache = (numeri *) DTIM_BASE;
static const size_t numeri_cnt = 8;

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

void numeri_copy(numeri *a, numeri *b) {
	uint32_t *awp = (uint32_t *)&a->data;
	uint32_t *bwp = (uint32_t *)&b->data;
	for (size_t word_idx = 0; word_idx < NUMERI_MAX_BYTES/sizeof(uint32_t); word_idx++) {
		bwp[word_idx] = awp[word_idx];
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

bool numeri_sub(numeri *a, numeri *b, numeri *c) {
	// We don't have negative numbers, so bail if that would happen
	if (numeri_is_bigger(b, a)) {
		return false;
	}

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

void numeri_mod(numeri *a, numeri *b, numeri *n) {
	// Zero result to avoid interference
	numeri_clean(b);

	if (numeri_size(a) < numeri_size(n)) {
		numeri_copy(a, b);
		return;
	}
}

void numeri_pow(numeri *a, uint32_t b, numeri *c, numeri *n) {
	// Zero result to avoid interference
	numeri_clean(c);

	numeri *curr_pow = numeri_alloc();
	uint32_t pow_lvl = 1;
}
