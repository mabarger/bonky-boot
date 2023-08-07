# required pip package: pycryptodome

from Crypto.PublicKey import RSA
from hashlib import sha256

# Key data
p = 0xb9ba48033658dd23f59255d731c9e969d660520b77e8605cbaa93b8b5dfe27c93864b2ddd09048ce8a8be7e60e1b27b543e438fcd262a5309938e423a543654b
q = 0xf95a6c850ec56bd399e042b965bce1b79eb9f7398e56aca0dccbebf264277ab94dee3ea33846c8eb434a4818edb33f484a006f966d13fd38cbad9156651017f9
n = 0xb4e7ca438fddb97269ad238158619ca0b5c7980bcfa9796c6c131984013fb07d5017c43b2a2455d9cd6b374ec38a2b109c89cce5085f8d1ce7f5f4708cd3bd5ffb7ae21e6a3c26ac07ec7610c0b135e05356da3eafc49e04d40c18bfa12dcc9e31134797b308b1c59da86886c2c0403d2748dad056294b7414f587e3b85742f3
d = 0x6d7c9b7da864990815cd23dff48d9f2de4d697bf0e3f93ad7afb5446003b7ba55c04064f8881d533ac8f6739d43e9b31c28be3e5f9db5a9c54071799c6a19fcaa026b71058066b57fe52432c7dbd9e3d4663819a986eae07a333f76fd67c2a7ff427d3f283aee51e40f90a1da2da5c91915482d356196b157cd8a25c3365351
e = 0x10001

# In case you want to generate a new keypair
#key_pair = RSA.generate(bits=1024)
key_pair = RSA.construct((n, e, d, p, q))

# Data to sign
data = b"\x87\x97\xa6\x54\x97\x98\x14\x9a\x6e\xb8\x90\xad\x46\x48\x93\x54\xf3\x8d\xcd\x8e\x99\x4b\x52\x84\xd0\xa5\x0e\x4d\x6a\xfa\x05\xd2"

signature = pow(int.from_bytes(data, byteorder='big'), key_pair.d, key_pair.n)

print("Signature:")
print(hex(signature))
