#include <sha512.h>

#include <string.h>

static const eit_u64_t eih_IV[8];
static const eit_u64_t eih_K[80];

static eit_u64_t eih_byteswap(eit_u64_t n) {
#if __GNUC__
    return __builtin_bswap64(n);
#else
    n = (n & EIT_U64_C(0xFFFFFFFF00000000)) >> 32 | (n & EIT_U64_C(0x00000000FFFFFFFF)) << 32;
    n = (n & EIT_U64_C(0xFFFF0000FFFF0000)) >> 16 | (n & EIT_U64_C(0x0000FFFF0000FFFF)) << 16;
    n = (n & EIT_U64_C(0xFF00FF00FF00FF00)) >>  8 | (n & EIT_U64_C(0x00FF00FF00FF00FF)) <<  8;
    return n;
#endif
}

static eit_u64_t eih_ror(eit_u64_t n, int s) {
    return n >> s | n << (-s & 63);
}

static void eih_turn(eih_context_t* ctx) {
    eit_u64_t A, B, C, D, E, F, G, H;
    eit_u64_t S0, S1, Ch, Ma, T1, T2;
    eit_u64_t W[80]; size_t i;

    memcpy(W, ctx->buffer, sizeof ctx->buffer);
    memset(ctx->buffer, 0, sizeof ctx->buffer);
    ctx->buflen = 0;

    for (i =  0; i < 16; i++) W[i] = eih_byteswap(W[i]);
    for (i = 16; i < 80; i++) {
        S0 = eih_ror(W[i - 15],  1) ^ eih_ror(W[i - 15],  8) ^ (W[i - 15] >> 7);
        S1 = eih_ror(W[i -  2], 19) ^ eih_ror(W[i -  2], 61) ^ (W[i -  2] >> 6);

        W[i] = W[i - 16] + S0 + W[i - 7] + S1;
    }

    A = ctx->hash[0]; B = ctx->hash[1]; C = ctx->hash[2]; D = ctx->hash[3];
    E = ctx->hash[4]; F = ctx->hash[5]; G = ctx->hash[6]; H = ctx->hash[7];

    for (i = 0; i < 80; i++) {
        S0 = eih_ror(A, 28) ^ eih_ror(A, 34) ^ eih_ror(A, 39);
        S1 = eih_ror(E, 14) ^ eih_ror(E, 18) ^ eih_ror(E, 41);

        Ch = (E & F) ^ (~E & G);
        Ma = (A & B) ^ (B & C) ^ (C & A);

        T1 = H + S1 + Ch + eih_K[i] + W[i];
        T2 = S0 + Ma;

        H = G; G = F; F = E; E = T1 + D ;
        D = C; C = B; B = A; A = T1 + T2;
    }

    ctx->hash[0] += A; ctx->hash[1] += B; ctx->hash[2] += C; ctx->hash[3] += D;
    ctx->hash[4] += E; ctx->hash[5] += F; ctx->hash[6] += G; ctx->hash[7] += H;
}

void eih_init(eih_context_t* ctx) {
    memset(ctx, 0, sizeof *ctx);
    memcpy(ctx->hash, eih_IV, sizeof eih_IV);
}

void eih_next(eih_context_t* ctx, const void* data, size_t count) {
    while (count > 0) {
        size_t min = sizeof ctx->buffer - ctx->buflen < count
                   ? sizeof ctx->buffer - ctx->buflen : count;

        memcpy(ctx->buffer + ctx->buflen, data, min);
        data = (const eit_byte_t*)data + min;
        ctx->buflen += min; count -= min;

        ctx->len[0] >>= 3;
        ctx->len[0] += min;
        ctx->len[1] += ctx->len[0] >> 61;
        ctx->len[0] <<= 3;

        if (ctx->buflen >= sizeof ctx->buffer) eih_turn(ctx);
    }
}

void eih_halt(eih_context_t* ctx, eih_hash_t* hash) {
    const size_t offset = sizeof ctx->buffer - sizeof ctx->len;
    eit_u64_t temp; size_t i;

    ctx->buffer[ctx->buflen++] = 0x80;
    if (ctx->buflen > offset) eih_turn(ctx);

    temp = ctx->len[0];
    ctx->len[0] = eih_byteswap(ctx->len[1]);
    ctx->len[1] = eih_byteswap(temp);
    memcpy(ctx->buffer + offset, ctx->len, sizeof ctx->len);
    eih_turn(ctx);

    for (i = 0; i < 8; i++)
        ctx->hash[i] = eih_byteswap(ctx->hash[i]);
    memcpy(hash->byte, ctx->hash, sizeof hash->byte);
}

static const eit_u64_t eih_IV[8] = {
    EIT_U64_C(0x6a09e667f3bcc908), EIT_U64_C(0xbb67ae8584caa73b), EIT_U64_C(0x3c6ef372fe94f82b), EIT_U64_C(0xa54ff53a5f1d36f1),
    EIT_U64_C(0x510e527fade682d1), EIT_U64_C(0x9b05688c2b3e6c1f), EIT_U64_C(0x1f83d9abfb41bd6b), EIT_U64_C(0x5be0cd19137e2179)
};

static const eit_u64_t eih_K[80] = {
    EIT_U64_C(0x428a2f98d728ae22), EIT_U64_C(0x7137449123ef65cd), EIT_U64_C(0xb5c0fbcfec4d3b2f), EIT_U64_C(0xe9b5dba58189dbbc),
    EIT_U64_C(0x3956c25bf348b538), EIT_U64_C(0x59f111f1b605d019), EIT_U64_C(0x923f82a4af194f9b), EIT_U64_C(0xab1c5ed5da6d8118),
    EIT_U64_C(0xd807aa98a3030242), EIT_U64_C(0x12835b0145706fbe), EIT_U64_C(0x243185be4ee4b28c), EIT_U64_C(0x550c7dc3d5ffb4e2),
    EIT_U64_C(0x72be5d74f27b896f), EIT_U64_C(0x80deb1fe3b1696b1), EIT_U64_C(0x9bdc06a725c71235), EIT_U64_C(0xc19bf174cf692694),
    EIT_U64_C(0xe49b69c19ef14ad2), EIT_U64_C(0xefbe4786384f25e3), EIT_U64_C(0x0fc19dc68b8cd5b5), EIT_U64_C(0x240ca1cc77ac9c65),
    EIT_U64_C(0x2de92c6f592b0275), EIT_U64_C(0x4a7484aa6ea6e483), EIT_U64_C(0x5cb0a9dcbd41fbd4), EIT_U64_C(0x76f988da831153b5),
    EIT_U64_C(0x983e5152ee66dfab), EIT_U64_C(0xa831c66d2db43210), EIT_U64_C(0xb00327c898fb213f), EIT_U64_C(0xbf597fc7beef0ee4),
    EIT_U64_C(0xc6e00bf33da88fc2), EIT_U64_C(0xd5a79147930aa725), EIT_U64_C(0x06ca6351e003826f), EIT_U64_C(0x142929670a0e6e70),
    EIT_U64_C(0x27b70a8546d22ffc), EIT_U64_C(0x2e1b21385c26c926), EIT_U64_C(0x4d2c6dfc5ac42aed), EIT_U64_C(0x53380d139d95b3df),
    EIT_U64_C(0x650a73548baf63de), EIT_U64_C(0x766a0abb3c77b2a8), EIT_U64_C(0x81c2c92e47edaee6), EIT_U64_C(0x92722c851482353b),
    EIT_U64_C(0xa2bfe8a14cf10364), EIT_U64_C(0xa81a664bbc423001), EIT_U64_C(0xc24b8b70d0f89791), EIT_U64_C(0xc76c51a30654be30),
    EIT_U64_C(0xd192e819d6ef5218), EIT_U64_C(0xd69906245565a910), EIT_U64_C(0xf40e35855771202a), EIT_U64_C(0x106aa07032bbd1b8),
    EIT_U64_C(0x19a4c116b8d2d0c8), EIT_U64_C(0x1e376c085141ab53), EIT_U64_C(0x2748774cdf8eeb99), EIT_U64_C(0x34b0bcb5e19b48a8),
    EIT_U64_C(0x391c0cb3c5c95a63), EIT_U64_C(0x4ed8aa4ae3418acb), EIT_U64_C(0x5b9cca4f7763e373), EIT_U64_C(0x682e6ff3d6b2b8a3),
    EIT_U64_C(0x748f82ee5defb2fc), EIT_U64_C(0x78a5636f43172f60), EIT_U64_C(0x84c87814a1f0ab72), EIT_U64_C(0x8cc702081a6439ec),
    EIT_U64_C(0x90befffa23631e28), EIT_U64_C(0xa4506cebde82bde9), EIT_U64_C(0xbef9a3f7b2c67915), EIT_U64_C(0xc67178f2e372532b),
    EIT_U64_C(0xca273eceea26619c), EIT_U64_C(0xd186b8c721c0c207), EIT_U64_C(0xeada7dd6cde0eb1e), EIT_U64_C(0xf57d4f7fee6ed178),
    EIT_U64_C(0x06f067aa72176fba), EIT_U64_C(0x0a637dc5a2c898a6), EIT_U64_C(0x113f9804bef90dae), EIT_U64_C(0x1b710b35131c471b),
    EIT_U64_C(0x28db77f523047d84), EIT_U64_C(0x32caab7b40c72493), EIT_U64_C(0x3c9ebe0a15c9bebc), EIT_U64_C(0x431d67c49c100d4c),
    EIT_U64_C(0x4cc5d4becb3e42b6), EIT_U64_C(0x597f299cfc657e2a), EIT_U64_C(0x5fcb6fab3ad6faec), EIT_U64_C(0x6c44198c4a475817)
};