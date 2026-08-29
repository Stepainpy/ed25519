from random import randint
import sys

PRIME: int = 2**255 - 19
CCD: int = 37095705934669439343138083508754565189542113879843219016388785533085940283555

modp_sqrt_m1 = pow(2, (PRIME - 1) // 4, PRIME)

def modp_inv(x):
    return pow(x, PRIME - 2, PRIME)

def recover_x(y, sign):
    if y >= PRIME:
        return None
    x2 = (y*y-1) * modp_inv(CCD*y*y+1)
    if x2 == 0:
        if sign:
            return None
        else:
            return 0

    # Compute square root of x2
    x = pow(x2, (PRIME + 3) // 8, PRIME)
    if (x*x - x2) % PRIME != 0:
        x = x * modp_sqrt_m1 % PRIME
    if (x*x - x2) % PRIME != 0:
        return None

    if (x & 1) != sign:
        x = PRIME - x
    return x

def get_point() -> tuple[int, int, int, int]:
    while True:
        y = randint(0, (1 << 256) - 1)
        sign = y >> 255
        y &= (1 << 255) - 1
        x = recover_x(y, sign)
        if x is not None:
            return (x, y, 1, x * y % PRIME)

def add_point(P: tuple[int, int, int, int], Q: tuple[int, int, int, int]) -> tuple[int, int, int, int]:
    A = (P[1] - P[0]) * (Q[1] - Q[0]) % PRIME
    B = (P[1] + P[0]) * (Q[1] + Q[0]) % PRIME
    C = 2 * CCD * P[3] * Q[3] % PRIME
    D = 2       * P[2] * Q[2] % PRIME
    E, F = (B - A) % PRIME, (D - C) % PRIME
    H, G = (B + A) % PRIME, (D + C) % PRIME
    return (
        E * F % PRIME,
        G * H % PRIME,
        F * G % PRIME,
        E * H % PRIME
    )

def times_point(P: tuple[int, int, int, int], n: int) -> tuple[int, int, int, int]:
    Q = (0, 1, 1, 0)
    while n > 0:
        if n & 1:
            Q = add_point(Q, P)
        P = add_point(P, P)
        n >>= 1
    return Q

def print_point(P, e):
    print(f"{P[0]:064x}{P[1]:064x}{P[2]:064x}{P[3]:064x}", end=e)

print('0/1000000', file=sys.stderr, end='')
for i in range(1_000_000):
    n = randint(0, (1 << 255) - 1)
    P = get_point()
    Q = times_point(P, n)
    print(f"\r{i+1}/1000000", file=sys.stderr, end='')
    print(f"{n:064x} ", end='')
    print_point(P, ' ')
    print_point(Q, '\n')
print(file=sys.stderr)