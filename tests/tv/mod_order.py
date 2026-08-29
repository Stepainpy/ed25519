from random import randint

ORDER: int = 2**252 + 27742317777372353535851937790883648493

for i in range(1_000_000):
    r = randint(0, (1 << 512) - 1)
    c = r % ORDER
    print(f"{r:0128x} {c:064x}")