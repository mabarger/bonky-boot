#include <types.h>

#define RSA_KEY_SIZE (128)
#define SHA256_DIGEST_SIZE (32)

/* Verifies the given signature by comparing against the hash */
bool rsa_verify(uint8_t *signature, uint8_t *hash);
