# bonky-boot
`bonky-boot` is a lightweight secure boot implementation for the RISC-V ISA, specifically the `FE310-G002` chip. It performs an RSA signature check to ensure the integrity and authenticity of the next software in the chain, before transferring execution to it.

To ensure the integrity of the public key and the code used to verify the signature, we place the whole implementation in the OTP (one-time programmable) ROM on the chip. As this RAM can be programmed exactly one time by burning a fuse, both code and key are protected from modifications.

A limitation to consider here is that this OTP memory only has 8KiB in size, imposing size constraints upon the software as the whole `bonky-boot` has to fit within these 8KiB. Memory-efficient design is therefore the core principle of this implementation, which is 4KiB big in total, so plenty of space left in those 8KiB for e.g. a spi display driver.

## SHA256
The `sha256` has been written from scratch directly in assembly. It uses as little RAM as possible and exploits the larger availability of registers compared to other architectures. It uses 32 bytes for the initial hash constants that get replaced with the computed ones at the end of the processing of every block, 256 bytes for the 64 `k` words that are immutable and 64 bytes for the `state`. While usual implemenetations allocate 64 words, one for each of the rounds, this one uses only 16 words, progressively replacing the older ones since they are no longer needed. Furthermore, the hashing context is just plain registers, thus no extra memory read or write operations happens.

## RSA
The RSA implementation takes in the signature, the public exponent and the modulus to compute the message hash, which will then be compared against the hash of the software of the next stage. If the hashes are equal, then the signature is valid and `true` will be returned, otherwise `false` will be returned to indicate that the signature is invalid.

## Big Integers (numeri)
As RSA requires arbitrary precision integers to perform the computations for reasonable key sizes, an arbitrary precision integer library was needed. After testing multiple available and stable implementations we realized that all available libraries were either not well usable in the embedded field or simply way too big in terms of memory requirements (>=4KB). `bonky-boot` therefore uses our own arbitrary precision integer implementation `numeri`, which is inspired by other libraries and optimized for memory usage and the RSA use case.
