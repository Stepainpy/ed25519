#ifndef ED25519_CONSTANTS_H
#define ED25519_CONSTANTS_H

#include <types.h>

static const eit_u256_t EIC_PRIME[1] = {{{
    0xffffffffffffffed, 0xffffffffffffffff,
    0xffffffffffffffff, 0x7fffffffffffffff
}}};

static const eit_u256_t EIC_ORDER[1] = {{{
    0x5812631a5cf5d3ed, 0x14def9dea2f79cd6,
    0x0000000000000000, 0x1000000000000000
}}};

static const eit_u256_t EIC_CURVE_CONST_D[1] = {{{
    0x75eb4dca135978a3, 0x00700a4d4141d8ab,
    0x8cc740797779e898, 0x52036cee2b6ffe73
}}};

static const eit_point_t EIC_CURVE_POINT_B[1] = {{
    {{ 0xc9562d608f25d51a, 0x692cc7609525a7b2, 0xc0a4e231fdd6dc5c, 0x216936d3cd6e53fe }},
    {{ 0x6666666666666658, 0x6666666666666666, 0x6666666666666666, 0x6666666666666666 }},
    {{                  1,                  0,                  0,                  0 }},
    {{ 0x6dde8ab3a5b7dda3, 0x20f09f80775152f5, 0x66ea4e8e64abe37d, 0x67875f0fd78b7665 }}
}};

/* Derived */

static const eit_u256_t EIC_NEG_PRIME[1] = {{{
    0x13, 0, 0, 0x8000000000000000
}}};

static const eit_u256_t EIC_PRIME_SUB_5_SHR_3[1] = {{{
    0xfffffffffffffffd, 0xffffffffffffffff,
    0xffffffffffffffff, 0x0fffffffffffffff
}}};

static const eit_u256_t EIC_PRIME_SUB_2[1] = {{{
    0xffffffffffffffeb, 0xffffffffffffffff,
    0xffffffffffffffff, 0x7fffffffffffffff
}}};

static const eit_u256_t EIC_ORDER_SHL_3[1] = {{{
    0xc09318d2e7ae9f68, 0xa6f7cef517bce6b2,
    0x0000000000000000, 0x8000000000000000
}}};

static const eit_u256_t EIC_2_POW_PRIME_SUB_1_SHR_2[1] = {{{
    0xc4ee1b274a0ea0b0, 0x2f431806ad2fe478,
    0x2b4d00993dfbd7a7, 0x2b8324804fc1df0b
}}};

#endif /* ED25519_CONSTANTS_H */