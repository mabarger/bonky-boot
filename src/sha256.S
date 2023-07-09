.data
.global sha256
.balign 32

k:
	.word 0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da, 0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070, 0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
h:
	.word 0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19, 0, 0, 0, 0, 0, 0, 0, 0

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

# use t0, t1, t2, returns t0
.macro little_to_big register
	#lw \register, 0(\address)
	mv t1, \register
	mv t2, x0
	srli t1, t1, 24
	or t2, t2, t1
	mv t1, \register
	slli t1, t1, 8
	srli t1, t1, 24
	slli t1, t1, 8
	or t2, t2, t1
	mv t1, \register
	slli t1, t1, 16
	srli t1, t1, 24
	slli t1, t1, 16
	or t2, t2, t1
	mv t1, \register
	slli t1, t1, 24
	or t2, t2, t1
	mv t0, t2
.endm

# t0, t1, t2, returns t0
.macro sigma0 x
	rotate_right \x, 7
	mv t2, t0
	rotate_right \x, 18
	xor t2, t2, t0
	srli t0, \x, 3
	xor t0, t2, t0
.endm

# t0, t1, t2, returns t0
.macro sigma1 x
	rotate_right \x, 17
	mv t2, t0
	rotate_right \x, 19
	xor t2, t2, t0
	srli t0, \x, 10
	xor t0, t2, t0
.endm

# t0, t1, t2, returns t0
.macro epsilon0 x
	rotate_right \x, 2
	mv t2, t0
	rotate_right \x, 13
	xor t2, t2, t0
	rotate_right \x, 22
	xor t0, t2, t0
.endm

# t0, t1, t2, returns t0
.macro epsilon1 x
	rotate_right \x, 6
	mv t2, t0
	rotate_right \x, 11
	xor t2, t2, t0
	rotate_right \x, 25
	xor t0, t2, t0
.endm

# t0, t1, returns t0
.macro maj x, y, z
	# (𝑥∧𝑦)⊕(𝑥∧𝑧)⊕(𝑦∧𝑧)
	and t0, \x, \y
	and t1, \x, \z
	xor t0, t0, t1
	and t1, \y, \z
	xor t0, t0, t1
.endm 

# t0, t1, t2, returns t0
.macro ch x, y, z
	# (𝑥∧𝑦)⊕(¬𝑥∧𝑧)
	and t0, \x, \y
	xor t1, \x, -1
	and t2, t1, \z
	xor t0, t2, t0
.endm

# load the initial value of the hash, from now on
# we rely only on the registers
.macro sha256_init a, b, c, d, e, f, g, h
	la t0, h
	lw \a, 0(t0)
	lw \b, 4(t0)
	lw \c, 8(t0)
	lw \d, 12(t0)
	lw \e, 16(t0)
	lw \f, 20(t0)
	lw \g, 24(t0)
	lw \h, 28(t0)
	# zero them so we can reuse this ram as state
	sw x0, 0(t0)
	sw x0, 4(t0)
	sw x0, 8(t0)
	sw x0, 12(t0)
	sw x0, 16(t0)
	sw x0, 20(t0)
	sw x0, 24(t0)
	sw x0, 28(t0)
.endm

.macro sha256_save a, b, c, d, e, f, g, h, destination
	sw \a, 0(\destination)
	sw \b, 4(\destination)
	sw \c, 8(\destination)
	sw \d, 12(\destination)
	sw \e, 16(\destination)
	sw \f, 20(\destination)
	sw \g, 24(\destination)
	sw \h, 28(\destination)
.endm

# uses sigma0, sigma1, ch, maj (can't use t0-t2)
# uses t3-t4, does not returns and updates the initial values
.macro sha256_update a, b, c, d, e, f, g, h, k, w
	# t3 is T1, t4 is T2
	## T1
	# load h in T1
	mv t3, \h
	# epsilon1(e)
	epsilon1 \e
	# add epsilon1 to T1
	add t3, t0, t3
	# ch(e, f, g)
	ch \e \f \g
	# add ch to T1
	add t3, t0, t3
	# add k to T1
	add t3, \k, t3
	# add w to T1
	add t3, \w, t3
	## T2
	add t4, x0, x0
	# epsilon0(a)
	epsilon0 \a
	# add epsilon1 to T2
	add t4, t0, t4
	# maj(a, b, c)
	maj \a, \b, \c
	# add maj to T2
	add t4, t0, t4

	# start swapping registers, can use again t0, t1, t2 as temporary if needed
	# h = g
	mv \h, \g
	# g = f
	mv \g, \f
	# f = e
	mv \f, \e
	# e = d + T1
	mv \e, \d
	add \e, t3, \e
	# d = c
	mv \d, \c
	# c = b
	mv \c, \b
	# b = a
	mv \b, \a
	# a = T1+T2
	mv \a, t3
	add \a, t4, \a
.endm

# uses t0, t1, t2, t3, returns t0
.macro load_and_pad_last_word address, length
	# find, align and pad the data end
	# example: 9bytes
	# load the divisor
	li t3, 4
	# get the leftover bytes
	# 9%4=1 (t3=1, a1=9)
	rem t3, \length, t3
	# get the aligned size
	# 9-1=8, (t1=8)
	#sub t1, \length, t3
	# add the aligned size at the starting address
	# load in t5
	#add t2, t1, \address
	# and its the length in t0
	#lw t0, 0(t2)
	lw t0, 0(\address)
	little_to_big t0
	# we need to zero out the extra data
	# and add the famous 1 bit
	# get a register that is 0xFFFFFFFF
	mv t1, x0
	xor t1, t1, -1
	# calculate the shift needed
	li t2, 4
	sub t2, t2, t3
	slli t2, t2, 3
	sll t1, t1, t2
	# clean extra bytes
	and t0, t0, t1
	# add the final 1
	mv t1, x0
	addi t1, t1, 1
	sub t2, t2, t1
	sll t1, t1, t2
	or t0, t0, t1
.endm

sha256:
	### arguments ###
	# a0 start address, a1 total size, a2 hash destination
	### registers usage ###
	# s1 = total size for final padding
	# s2 = a
	# s3 = b
	# s4 = c
	# s5 = d
	# s6 = e
	# s7 = f
	# s8 = g
	# s9 = h
	## save total size (it is already provided in byes)
	## a1 is instead decremented while processing
	mv s1, a1
	## init hash registers with the h values
	sha256_init s2, s3, s4, s5, s6, s7, s8, s9

	outer_loop:
		# is it the last block? 55 bytes = 440 bits
		li t0, 55
		ble a1, t0, final_block

		intermediate_block:

		# increase start address
		# decrease left counter
		j round_loop

		final_block:
			li t0, 4
			# how many blocks of data? in t5
			div t3, a1, t0
			# load the target of the 64 bytes state
			la t4, h

			populate_data:
				# load te current 4 bye word
				lw t0, 0(a0)
				# swap endianess
				little_to_big t0
				# store the word with the new endianess in the target
				sw t0, 0(t4)
				# move the target state address
				addi t4, t4, 4
				# move the source address
				addi a0, a0, 4
				# decrement the leftover words counter
				add t3, t3, -1
				# loop if there is still data
				bnez t3, populate_data
				
				# we could load x0 in all the other addresses
				# but they are already initialized at 0

			load_and_pad_last_word a0, a1
			sw t0, 0(t4)
			# why don't we switch endianess here?
			#little_to_big s1
			la t4, h
			# size is in bits so multiply first
			slli t0, s1, 3
			sw t0, 60(t4)

		hashing:
			# here we start the hashing process
			# h is already in t4
			la t5, h
			la t6, k
			# initalize round counter
			mv a7, x0
			round_loop:
				addi t0, x0, 65
				beq t0, a7, round_end
				# load w
				lw a3, 0(t5)
				# load k
				lw a4, 0(t6)

				sha256_update s2, s3, s4, s5, s6, s7, s8, s9, a4, a3
		
				# increment counter
				addi a7, a7, 1
				# increment w pointer but wrap around
				addi t5, t5, 4
				# increment k pointer
				addi t6, t6, 4
				j round_loop
			round_end:



	## save the final hash back to the provided ptr
	sha256_save s2, s3, s4, s5, s6, s7, s8, s9, a2
	ret