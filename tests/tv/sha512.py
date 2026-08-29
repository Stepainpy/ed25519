from random import randbytes
import hashlib

def sha512(msg):
    return hashlib.sha512(msg).digest()

def print_bytes(b, e):
    for i in range(len(b)):
        print(f"{b[i]:02x}", end='')
    print(end=e)

for i in range(1_000_000):
    buf = randbytes(32)
    hash = sha512(buf)
    print_bytes(buf, ' ')
    print_bytes(hash, '\n')