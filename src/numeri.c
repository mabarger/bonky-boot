#include <memory.h>
#include <numeri.h>

/* Cache that contains the available big numbers*/
static numeri *numeri_cache = (numeri *) DTIM_BASE;
static const size_t numeri_cnt = 8;

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
			curr_numeri->data[0] = 0x24;
			curr_numeri->data[1] = 0xF5;
			curr_numeri->data[2] = 0x72;
			curr_numeri->data[3] = 0x21;
			curr_numeri->data[4] = 0x93;
			return curr_numeri;
		}
		curr_numeri++;
	}
	return NULL;
}

void numeri_free(numeri *a) {
	a->used = false;
}

static void numeri_clean(numeri *a) {
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

void numeri_mul(numeri *a, numeri *b, numeri *c) {
	// Zero result to avoid interference
	numeri_clean(c);

	// Multiply numeris byte by byte karasuba-style
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
