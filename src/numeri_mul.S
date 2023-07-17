.globl mp_mul

/* Multi-Precision multiplication */
/* Signature: mp_mul(uint32_t a, uint32_t b, uint32_t carry), returns result in a0 and carry in a3*/
mp_mul:
	// Get low and high multiplication result
	mulhu a3, a0, a1
	mul   a0, a0, a1

	// Add carry from previous word
	add  a0, a0, a2
	sltu a4, a0, a2
	add a3, a3, a4
	ret
