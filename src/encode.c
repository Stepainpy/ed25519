#include <encode.h>
#include <arbmath.h>
#include <constants.h>

#include <string.h>

void eie_encode_point(eit_byte_t* dst, const eit_point_t* P) {
    eit_u256_t invZ[1], x[1], y[1];

    eim_pow_mod_p_u256(invZ, &P->z, EIC_PRIME_SUB_2);

    eim_mul_mod_p_u256(x, &P->x, invZ);
    eim_mul_mod_p_u256(y, &P->y, invZ);

    y->u[3] &= (EIT_U64_C(1) << 63) - 1;
    y->u[3] |= x->u[0] << 63;

    memcpy(dst, y->u, sizeof y);
}

int eie_decode_point(eit_point_t* P, const eit_byte_t* src) {
    eit_u256_t x2[1], y2[1], tmp[1] = {0}; eit_u64_t x_0;
    eit_u256_t x[1], y[1], u[1], v[1], uv[1];

    /* get y and x_0 */

    memcpy(y, src, sizeof y);
    x_0 = y->u[3] >> 63;
    y->u[3] &= (EIT_U64_C(1) << 63) - 1;

    if (eim_cmp_u256(y, EIC_PRIME) >= 0) return EIE_FAIL;

    /* calc u = y^2 - 1 and v = d y^2 + 1 */

    eim_mul_mod_p_u256(y2, y, y);

    tmp->u[0] = 1;
    eim_sub_mod_p_u256(u, y2, tmp);

    eim_mul_mod_p_u256(y2, y2, EIC_CURVE_CONST_D);
    eim_add_mod_p_u256(v, y2, tmp);

    /* calc x = u (u v)^((p - 5) / 8) */

    eim_mul_mod_p_u256(uv, u, v);
    eim_pow_mod_p_u256(uv, uv, EIC_PRIME_SUB_5_SHR_3);
    eim_mul_mod_p_u256(x, u, uv);

    /* test v x^2 = u or -u */

    eim_mul_mod_p_u256(x2, x, x);
    eim_mul_mod_p_u256(x2, v, x2);

    if (eim_cmp_u256(x2, u) != 0) {
        tmp->u[0] = 0;
        eim_sub_mod_p_u256(u, tmp, u);
        if (eim_cmp_u256(x2, u) != 0) return EIE_FAIL;
        eim_mul_mod_p_u256(x, x, EIC_2_POW_PRIME_SUB_1_SHR_2);
    }

    /* choose correct x */

    tmp->u[0] = 0;
    if (x_0 && eim_cmp_u256(x, tmp) == 0) return EIE_FAIL;

    if ((x->u[0] & 1) != x_0)
        eim_simple_sub_u256(x, EIC_PRIME, x);

    /* set point coords */

    memcpy(&P->x, x, sizeof x);
    memcpy(&P->y, y, sizeof y);
    memset(&P->z, 0, sizeof x); P->z.u[0] = 1;
    eim_mul_mod_p_u256(&P->t, x, y);

    return EIE_OKEY;
}