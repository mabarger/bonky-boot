# bonky-boot 🦦
`bonky-boot` is a lightweight secure boot implementation for the RISC-V ISA, specifically the [FE310-G002](https://starfivetech.com/uploads/fe310-g002-manual-v1p0.pdf) chip. It performs an RSA signature check to ensure the integrity and authenticity of the next software in the chain, before transferring execution to it.

To ensure the integrity of the public key and the code used to verify the signature, we place the whole implementation in the OTP (one-time programmable) ROM on the chip. As this OTP can be programmed exactly one time by burning a fuse, both code and key are protected from modifications.

A limitation to consider here is that this OTP memory only has 8KiB in size, imposing size constraints upon the software as the whole `bonky-boot` has to fit within these 8KiB. Memory-efficient design is therefore the core principle of this implementation, which is 4KiB big in total, so plenty of space left in those 8KiB for e.g. a spi display driver.

Flashing OTP memory is board dependant. For the FE310-G002, the procedure is described at page 54 of the manual:

  1. _LOCK_ the otp:
     1. Write `0x1` to `otp_lock`
     2. Check that `0x1` is read back from otp_lock.
     3. Repeat this step until `0x1` is read successfully.
  2. _SET_ the programming voltages by writing the following values:
     * `otp_mrr=0x4`
     * `otp_mpp=0x0`
     * `otp_vppen=0x0`
  4. _WAIT_ 20 us for the programming voltages to stabilize.
  5. ADDRESS the memory by setting `otp_a`.
  6. _WRITE_ one bit at a time:
     1. Set only the bit you want to write high in `otp_d`
     2. Bring `otp_ck` HIGH for 50 us
     3. Bring `otp_ck` LOW. Note that this means only one bit of `otp_d` should be high at any time.
  7. _VERIFY_ the written bits setting `otp_mrr=0x9` for read margin.
  8. _SOAK_ any verification failures by repeating steps 2-5 using 400 us pulses.
  9. _REVERIFY_ the rewritten bits setting `otp_mrr=0xF`. Steps 7, 8 may be repeated up to10 times before failing the part.
  10. _UNLOCK_ the otp by writing 0x0 to otp_lock.

## SHA256
The `sha256` has been written from scratch directly in assembly. The resulting stripped binary size is just under 2KiB, even though, if neded, it could be shrinked even further by replacing the most common macros with jumps. It uses as little RAM as possible and exploits the larger availability of registers compared to other architectures. It uses 32 bytes for the initial hash constants that get replaced with the computed ones at the end of the processing of every block, 256 bytes for the 64 `k` words that are immutable and 64 bytes for the `state`. While usual implemenetations allocate 64 words, one for each of the rounds, this one uses only 16 words, progressively replacing the older ones since they are no longer needed and bringing the total ram requirement to just 352 bytes. Furthermore, the hashing context is just plain registers, thus no extra memory read or write operations happens. Input is expected little endian, and is converted in order to perform the algorithm correctly. Output is converted back in little endian format to provide the expected result.

## RSA
The RSA implementation takes in the signature, the public exponent and the modulus to compute the message hash, which will then be compared against the hash of the software of the next stage. If the hashes are equal, then the signature is valid and `true` will be returned, otherwise `false` will be returned to indicate that the signature is invalid.

## Big Integers (numeri)
As RSA requires arbitrary precision integers to perform the computations for reasonable key sizes, an arbitrary precision integer library was needed. After testing multiple available and stable implementations we realized that all available libraries were either not well usable in the embedded field or simply way too big in terms of memory requirements (>=4KB). `bonky-boot` therefore uses our own arbitrary precision integer implementation `numeri`, which is inspired by other libraries and optimized for memory usage and the RSA use case.
