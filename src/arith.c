#include <arith.h>
#include <arbmath.h>
#include <constants.h>

#include <string.h>

void eia_add_point(eit_point_t* R, const eit_point_t* P, const eit_point_t* Q) {
    eit_u256_t a_h[8], t[2];

    /* A = (Py - Px) * (Qy - Qx) */
    eim_sub_mod_p_u256(t + 0, &P->y, &P->x);
    eim_sub_mod_p_u256(t + 1, &Q->y, &Q->x);
    eim_mul_mod_p_u256(a_h + 0, t + 0, t + 1);

    /* B = (Py + Px) * (Qy + Qx) */
    eim_add_mod_p_u256(t + 0, &P->y, &P->x);
    eim_add_mod_p_u256(t + 1, &Q->y, &Q->x);
    eim_mul_mod_p_u256(a_h + 1, t + 0, t + 1);

    /* C = 2 * d * Pt * Qt */
    eim_mul_mod_p_u256(t + 0, &P->t, &Q->t);
    eim_mul_mod_p_u256(t + 1, t + 0, EIC_CURVE_CONST_D);
    a_h[2].u[3] = t[1].u[3] << 1 | t[1].u[2] >> 63;
    a_h[2].u[2] = t[1].u[2] << 1 | t[1].u[1] >> 63;
    a_h[2].u[1] = t[1].u[1] << 1 | t[1].u[0] >> 63;
    a_h[2].u[0] = t[1].u[0] << 1                  ;
    if (eim_cmp_u256(a_h + 2, EIC_PRIME) >= 0)
        eim_simple_add_u256(a_h + 2, a_h + 2, EIC_NEG_PRIME);

    /* D = 2 * Pz * Qz */
    eim_mul_mod_p_u256(t + 1, &P->z, &Q->z);
    a_h[3].u[3] = t[1].u[3] << 1 | t[1].u[2] >> 63;
    a_h[3].u[2] = t[1].u[2] << 1 | t[1].u[1] >> 63;
    a_h[3].u[1] = t[1].u[1] << 1 | t[1].u[0] >> 63;
    a_h[3].u[0] = t[1].u[0] << 1                  ;
    if (eim_cmp_u256(a_h + 3, EIC_PRIME) >= 0)
        eim_simple_add_u256(a_h + 3, a_h + 3, EIC_NEG_PRIME);

    /* E = B - A */ eim_sub_mod_p_u256(a_h + 4, a_h + 1, a_h + 0);
    /* F = D - C */ eim_sub_mod_p_u256(a_h + 5, a_h + 3, a_h + 2);
    /* G = D + C */ eim_add_mod_p_u256(a_h + 6, a_h + 3, a_h + 2);
    /* H = B + A */ eim_add_mod_p_u256(a_h + 7, a_h + 1, a_h + 0);

    /* Rx = E * F */ eim_mul_mod_p_u256(&R->x, a_h + 4, a_h + 5);
    /* Ry = G * H */ eim_mul_mod_p_u256(&R->y, a_h + 6, a_h + 7);
    /* Rz = F * G */ eim_mul_mod_p_u256(&R->z, a_h + 5, a_h + 6);
    /* Rt = E * H */ eim_mul_mod_p_u256(&R->t, a_h + 4, a_h + 7);
}

void eia_times_point(eit_point_t* R, const eit_u256_t* n, const eit_point_t* P) {
    eit_point_t A[1];
    eit_u64_t mask;
    size_t i;

    memcpy(A, P, sizeof *P);
    memset(R, 0, sizeof *R);
    R->y.u[0] = R->z.u[0] = 1;

    for (i = 0, mask = 1; i < 256; i++) {
        if (n->u[i / 64] & mask)
            eia_add_point(R, R, A);
        eia_add_point(A, A, A);
        mask = mask << 1 | mask >> 63;
    }
}