#include <arbmath.h>
#include <constants.h>

#include <string.h>

int eim_cmp_uN(const eit_u64_t* lhs, const eit_u64_t* rhs, size_t n) {
    size_t i; for (i = n; i --> 0;) {
        if (lhs[i] > rhs[i]) return +1;
        if (lhs[i] < rhs[i]) return -1;
    }
    return 0;
}

static int eim_add_with_carry(eit_u64_t* z, eit_u64_t x, eit_u64_t y, int c) {
    eit_u64_t xy  = x  + y;
    eit_u64_t xyc = xy + c;
    *z = xyc;
    return (xy < x) || (xyc < xy);
}

static int eim_simple_carry_uN(eit_u64_t* val, int carry, size_t n) {
    size_t i; for (i = 0; carry && i < n; i++)
        carry = eim_add_with_carry(val + i, val[i], 0, carry);
    return carry;
}

static void eim_simple_neg_uN(eit_u64_t* ret, const eit_u64_t* val, size_t n) {
    size_t i; for (i = 0; i < n; i++) ret[i] = ~val[i];
    eim_simple_carry_uN(ret, 1, n);
}

int eim_simple_add_uN(eit_u64_t* ret, const eit_u64_t* lhs, const eit_u64_t* rhs, size_t n) {
    size_t i; int carry = 0;
    for (i = 0; i < n; i++)
        carry = eim_add_with_carry(
            ret + i, lhs[i], rhs[i], carry);
    return carry;
}

int eim_simple_sub_uN(eit_u64_t* ret, const eit_u64_t* lhs, const eit_u64_t* rhs, size_t n) {
    eit_u64_t neg[8];
           eim_simple_neg_uN(          neg, rhs, n);
    return eim_simple_add_uN(ret, lhs, neg,      n);
}

void eim_add_mod_p_u256(eit_u256_t* ret, const eit_u256_t* lhs, const eit_u256_t* rhs) {
    eim_simple_add_u256(ret, lhs, rhs);
    if (eim_cmp_u256(ret, EIC_PRIME) >= 0)
        eim_simple_add_u256(ret, ret, EIC_NEG_PRIME);
}

void eim_sub_mod_p_u256(eit_u256_t* ret, const eit_u256_t* lhs, const eit_u256_t* rhs) {
    if (eim_cmp_u256(lhs, rhs) >= 0)
        eim_simple_sub_u256(ret, lhs, rhs);
    else {
        eit_u256_t tmp[1];
        eim_simple_add_u256(tmp, lhs, EIC_PRIME);
        eim_simple_sub_u256(ret, tmp, rhs);
    }
}

static void eim_simple_mul_u64(eit_u128_t* ret, eit_u64_t x, eit_u64_t y) {
    eit_u64_t xh, xl, yh, yl, hl, lh;

    xh = x >> 32; xl = x & (~(eit_u32_t)0);
    yh = y >> 32; yl = y & (~(eit_u32_t)0);

    ret->u[0] = xl * yl; hl = xh * yl;
    ret->u[1] = xh * yh; lh = xl * yh;

    ret->u[1] += (hl >> 32) + eim_add_with_carry(ret->u, ret->u[0], hl << 32, 0);
    ret->u[1] += (lh >> 32) + eim_add_with_carry(ret->u, ret->u[0], lh << 32, 0);
}

static void eim_simple_mul_u128(eit_u256_t* ret, const eit_u128_t* lhs, const eit_u128_t* rhs) {
    eit_u64_t lsum, rsum; int cl, cr;
    eit_u128_t ll[1], hh[1], ss[1];
    eit_u256_t z[4] = {0};

    cl = eim_add_with_carry(&lsum, lhs->u[1], lhs->u[0], 0);
    cr = eim_add_with_carry(&rsum, rhs->u[1], rhs->u[0], 0);

    eim_simple_mul_u64(ll, lhs->u[0], rhs->u[0]);
    eim_simple_mul_u64(hh, lhs->u[1], rhs->u[1]);
    eim_simple_mul_u64(ss, lsum, rsum);

    memcpy(z[0].u, ll->u, sizeof ll);
    memcpy(z[2].u, hh->u, sizeof hh);
    memcpy(z[3].u, ss->u, sizeof ss);

    if (cr)
        eim_simple_carry_uN(z[3].u + 2,
            eim_add_with_carry(z[3].u + 1, z[3].u[1], lsum, 0), 2);
    if (cl)
        eim_simple_carry_uN(z[3].u + 2,
            eim_add_with_carry(z[3].u + 1, z[3].u[1], rsum, 0), 2);
    if (cl && cr)
        z[3].u[3] += eim_add_with_carry(z[3].u + 2, z[3].u[2], 1, 0);

    eim_simple_sub_u256(z + 1, z + 3, z + 2);
    eim_simple_sub_u256(z + 1, z + 1, z + 0);

    memmove(z[1].u + 1, z[1].u, sizeof(eit_u64_t) * 3); memset(z[1].u, 0, sizeof(eit_u64_t) * 1);
    memmove(z[2].u + 2, z[2].u, sizeof(eit_u64_t) * 2); memset(z[2].u, 0, sizeof(eit_u64_t) * 2);

    memset(ret, 0, sizeof *ret);
    eim_simple_add_u256(ret, ret, z + 0);
    eim_simple_add_u256(ret, ret, z + 1);
    eim_simple_add_u256(ret, ret, z + 2);
}

void eim_simple_mul_u256(eit_u512_t* ret, const eit_u256_t* lhs, const eit_u256_t* rhs) {
    eit_u128_t lsum[1], rsum[1]; int cl, cr;
    eit_u256_t ll[1], hh[1], ss[1];
    eit_u512_t z[4] = {0};

    cl = eim_simple_add_uN(lsum->u, lhs->u + 2, lhs->u, 2);
    cr = eim_simple_add_uN(rsum->u, rhs->u + 2, rhs->u, 2);

    eim_simple_mul_u128(ll, (const eit_u128_t*)lhs + 0, (const eit_u128_t*)rhs + 0);
    eim_simple_mul_u128(hh, (const eit_u128_t*)lhs + 1, (const eit_u128_t*)rhs + 1);
    eim_simple_mul_u128(ss, lsum, rsum);

    memcpy(z[0].u, ll->u, sizeof ll);
    memcpy(z[2].u, hh->u, sizeof hh);
    memcpy(z[3].u, ss->u, sizeof ss);

    if (cr) {
        int c = eim_simple_add_uN(z[3].u + 2, z[3].u + 2, lsum->u, 2);
        eim_simple_carry_uN(z[3].u + 4, c, 4);
    }
    if (cl) {
        int c = eim_simple_add_uN(z[3].u + 2, z[3].u + 2, rsum->u, 2);
        eim_simple_carry_uN(z[3].u + 4, c, 4);
    }
    if (cl && cr)
        eim_simple_carry_uN(z[3].u + 4, 1, 4);

    eim_simple_sub_u512(z + 1, z + 3, z + 2);
    eim_simple_sub_u512(z + 1, z + 1, z + 0);

    memmove(z[1].u + 2, z[1].u, sizeof(eit_u64_t) * 6); memset(z[1].u, 0, sizeof(eit_u64_t) * 2);
    memmove(z[2].u + 4, z[2].u, sizeof(eit_u64_t) * 4); memset(z[2].u, 0, sizeof(eit_u64_t) * 4);

    memset(ret, 0, sizeof *ret);
    eim_simple_add_u512(ret, ret, z + 0);
    eim_simple_add_u512(ret, ret, z + 1);
    eim_simple_add_u512(ret, ret, z + 2);
}

void eim_mul_mod_p_u256(eit_u256_t* ret, const eit_u256_t* lhs, const eit_u256_t* rhs) {
    eit_u512_t reg[1], sub[1] = {0}; size_t i, j;
    memcpy(sub->u + 4, EIC_PRIME, sizeof EIC_PRIME);

    eim_simple_mul_u256(reg, lhs, rhs);
    for (i = 0; i < 256 + 1; i++) {
        if (eim_cmp_u512(reg, sub) >= 0)
            eim_simple_sub_u512(reg, reg, sub);
        for (j = 0; j < 7; j++)
            sub->u[j] = sub->u[j] >> 1 | sub->u[j + 1] << 63;
        sub->u[7] >>= 1;
    }

    memcpy(ret, reg, sizeof *ret);
}

void eim_pow_mod_p_u256(eit_u256_t* ret, const eit_u256_t* lhs, const eit_u256_t* rhs) {
    eit_u256_t acc[1], reg[1] = {0};
    eit_u64_t mask; size_t i;

    memcpy(acc, lhs, sizeof acc);
    reg->u[0] = 1;

    for (i = 0, mask = 1; i < 256; i++) {
        if (rhs->u[i / 64] & mask)
            eim_mul_mod_p_u256(reg, reg, acc);
        eim_mul_mod_p_u256(acc, acc, acc);
        mask = mask << 1 | mask >> 63;
    }

    memcpy(ret, reg, sizeof reg);
}

void eim_mod_order_u512(eit_u256_t* ret, const eit_u512_t* val) {
    eit_u512_t reg[1], sub[1] = {0}; size_t i, j;

    memcpy(reg, val, sizeof reg);
    memcpy(sub->u + 4, EIC_ORDER_SHL_3, sizeof EIC_ORDER_SHL_3);

    for (i = 0; i < 256 + 4; i++) {
        if (eim_cmp_u512(reg, sub) >= 0)
            eim_simple_sub_u512(reg, reg, sub);
        for (j = 0; j < 7; j++)
            sub->u[j] = sub->u[j] >> 1 | sub->u[j + 1] << 63;
        sub->u[7] >>= 1;
    }

    memcpy(ret, reg, sizeof *ret);
}