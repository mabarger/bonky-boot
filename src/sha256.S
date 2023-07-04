.data
.globl k, h
.global sha256
.balign 32
k:
	.word 0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da, 0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070, 0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
h:
	.word 0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19

# t0, t1, returns t0
.macro rotate_left x, shift
slli t0, \x, \shift
srli t1, \x, (32 - \shift)
or t0, t0, t1
.endm

# t0, t1, returns t0
.macro rotate_right x, shift
srli t0, \x, \shift
slli t1, \x, (32 - \shift)
or t0, t0, t1
.endm

# t0, t1, t2, returns t0
.macro sigma0 x
rotate_right \x, 7
addi t2, t0, 0
rotate_right \x, 18
xor t2, t2, t0
srli t0, \x, 3
xor t0, t2, t0
.endm

# t0, t1, t2, returns t0
.macro sigma1 x
rotate_right \x, 17
addi t2, t0, 0
rotate_right \x, 19
xor t2, t2, t0
srli t0, \x, 10
xor t0, t2, t0
.endm

# t0, t1, t2, returns t0
.macro epsilon0 x
rotate_right \x, 2
addi t2, t0, 0
rotate_right \x, 13
xor t2, t2, t0
rotate_right \x, 22
xor t0, t2, t0
.endm

# t0, t1, t2, returns t0
.macro epsilon1 x
rotate_right \x, 6
addi t2, t0, 0
rotate_right \x, 11
xor t2, t2, t0
rotate_right \x, 25
xor t0, t2, t0
.endm

# t0, t1, returns t0
.macro maj x, y, z
and t0, \x, \y
and t1, \x, \z
xor t0, t0, t1
and t1, \y, \z
xor t0, t0, t1
.endm 

.macro ch x, y, z
and t0, \x, \y
# TODO
.endm

sha256:
addi a0, a0, 1