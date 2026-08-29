#include <ed25519/ed25519.h>
#include <constants.h>
#include <arbmath.h>
#include <random.h>
#include <sha512.h>
#include <encode.h>
#include <arith.h>
#include <types.h>

#include <string.h>

int ed25519_pvt_to_pub(ed25519_public_key_t pub_key, const ed25519_private_key_t pvt_key) {
    eih_context_t ctx[1]; eih_hash_t hash[1];
    eit_u256_t s[1]; eit_point_t A[1];

    if (!pub_key || !pvt_key) return ED25519_FAIL;

    eih_init(ctx);
    eih_next(ctx, pvt_key, ED25519_PRIVATE_KEY_SIZE);
    eih_halt(ctx, hash);
    memcpy(s, hash->byte, sizeof s);

    s->u[0] &= ~EIT_U64_C(7);
    s->u[3] &= (EIT_U64_C(1) << 63) - 1;
    s->u[3] |=  EIT_U64_C(1) << 62;

    eia_times_point(A, s, EIC_CURVE_POINT_B);

    eie_encode_point(pub_key, A);

    return ED25519_OKEY;
}

int ed25519_keygen(ed25519_private_key_t pvt_key, ed25519_public_key_t pub_key) {
    size_t i, count;

    if (!pvt_key || !pub_key) return ED25519_FAIL;

    i = 0; /* Generating private key, 3 attempts are given */
    do count = eir_generate(pvt_key, ED25519_PRIVATE_KEY_SIZE), i++;
    while (count != ED25519_PRIVATE_KEY_SIZE && i < 3);
    if (count != ED25519_PRIVATE_KEY_SIZE) return ED25519_FAIL;

    ed25519_pvt_to_pub(pub_key, pvt_key);

    return ED25519_OKEY;
}

int ed25519_sign(
    const void* data, size_t size,
    const ed25519_private_key_t pvt_key,
    ed25519_signature_t sign
) {
    eih_context_t ctx[1]; eih_hash_t hash[1];
    eit_u256_t s[1], r[1], k[1], S[1]; eit_u512_t eS[1];
    eit_byte_t eA[32], eR[32];
    eit_point_t A[1], R[1];

    if (!data || !pvt_key || !sign) return ED25519_FAIL;

    /* Generating scalar s and point A */

    eih_init(ctx);
    eih_next(ctx, pvt_key, ED25519_PRIVATE_KEY_SIZE);
    eih_halt(ctx, hash);
    memcpy(s, hash->byte, sizeof s);

    s->u[0] &= ~EIT_U64_C(7);
    s->u[3] &= (EIT_U64_C(1) << 63) - 1;
    s->u[3] |=  EIT_U64_C(1) << 62;

    eia_times_point(A, s, EIC_CURVE_POINT_B);
    eie_encode_point(eA, A);

    /* Generating scalar r and point R */

    eih_init(ctx);
    eih_next(ctx, hash->byte + 32, 32);
    eih_next(ctx, data, size);
    eih_halt(ctx, hash);

    eim_mod_order_u512(r, (const eit_u512_t*)hash->byte);
    eia_times_point(R, r, EIC_CURVE_POINT_B);
    eie_encode_point(eR, R);

    /* Generating scalar k */

    eih_init(ctx);
    eih_next(ctx, eR, sizeof eR);
    eih_next(ctx, eA, sizeof eA);
    eih_next(ctx, data, size);
    eih_halt(ctx, hash);

    eim_mod_order_u512(k, (const eit_u512_t*)hash->byte);

    /* Calculating of S */

    eim_simple_mul_u256(eS,  k,  s);
    eim_mod_order_u512(S, eS);
    eim_simple_add_u256(S, S, r);
    if (eim_cmp_u256(S, EIC_ORDER) >= 0)
        eim_simple_sub_u256(S, S, EIC_ORDER);

    /* Write signature */

    memcpy(sign +  0, eR, sizeof eR);
    memcpy(sign + 32,  S, sizeof  S);

    return ED25519_OKEY;
}

int ed25519_verify(
    const void* data, size_t size,
    const ed25519_public_key_t pub_key,
    const ed25519_signature_t sign
) {
    eih_context_t ctx[1]; eih_hash_t hash[1];
    eit_u256_t k[1], s[1], lhs[1], rhs[1], d[1];
    eit_point_t A[1], R[1], S[1];
    eit_byte_t eA[32], eR[32];

    /* Decoding points and scalar S (not secret from private key) */

    memcpy(eA, pub_key, sizeof eA);
    memcpy(eR, sign   , sizeof eR);
    memcpy(s, sign + 32, sizeof s);

    if (eim_cmp_u256(s, EIC_ORDER) >= 0) return ED25519_FAIL;

    if (eie_decode_point(A, eA)) return ED25519_FAIL;
    if (eie_decode_point(R, eR)) return ED25519_FAIL;

    /* Generating scalar k */

    eih_init(ctx);
    eih_next(ctx, eR, sizeof eR);
    eih_next(ctx, eA, sizeof eA);
    eih_next(ctx, data, size);
    eih_halt(ctx, hash);

    eim_mod_order_u512(k, (const eit_u512_t*)hash->byte);

    /* Checking equation sB = R + kA */

    eia_times_point(S, s, EIC_CURVE_POINT_B);
    eia_times_point(A, k, A);
    eia_add_point(R, R, A);

    eim_mul_mod_p_u256(lhs, &S->x, &R->z);
    eim_mul_mod_p_u256(rhs, &R->x, &S->z);
    eim_sub_mod_p_u256(d, lhs, rhs);
    if (d->u[0] || d->u[1] || d->u[2] || d->u[3]) return ED25519_FAIL;

    eim_mul_mod_p_u256(lhs, &S->y, &R->z);
    eim_mul_mod_p_u256(rhs, &R->y, &S->z);
    eim_sub_mod_p_u256(d, lhs, rhs);
    if (d->u[0] || d->u[1] || d->u[2] || d->u[3]) return ED25519_FAIL;

    return ED25519_OKEY;
}