from random import randint

for i in range(1_000_000):
    a = randint(0, (1 << 256) - 1)
    b = randint(0, (1 << 256) - 1)
    c = a * b
    print(f"{a:064x} {b:064x} {c:0128x}")