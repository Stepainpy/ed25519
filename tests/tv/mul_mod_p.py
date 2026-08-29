from random import randint

PRIME: int = 2**255 - 19

for i in range(1_000_000):
    a = randint(0, PRIME - 1)
    b = randint(0, PRIME - 1)
    c = (a * b) % PRIME
    print(f"{a:064x} {b:064x} {c:064x}")