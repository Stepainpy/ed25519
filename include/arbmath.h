#ifndef ED25519_ARBITRARY_MATH_H
#define ED25519_ARBITRARY_MATH_H

#include <types.h>

#include <stddef.h>

int eim_cmp_uN(const eit_u64_t* lhs, const eit_u64_t* rhs, size_t n);

int eim_simple_add_uN(eit_u64_t* ret, const eit_u64_t* lhs, const eit_u64_t* rhs, size_t n);
int eim_simple_sub_uN(eit_u64_t* ret, const eit_u64_t* lhs, const eit_u64_t* rhs, size_t n);

#define eim_cmp_u256(lhs, rhs) eim_cmp_uN((lhs)->u, (rhs)->u, 4)
#define eim_cmp_u512(lhs, rhs) eim_cmp_uN((lhs)->u, (rhs)->u, 8)

#define eim_simple_add_u256(ret, lhs, rhs) eim_simple_add_uN((ret)->u, (lhs)->u, (rhs)->u, 4)
#define eim_simple_add_u512(ret, lhs, rhs) eim_simple_add_uN((ret)->u, (lhs)->u, (rhs)->u, 8)

#define eim_simple_sub_u128(ret, lhs, rhs) eim_simple_sub_uN((ret)->u, (lhs)->u, (rhs)->u, 2)
#define eim_simple_sub_u256(ret, lhs, rhs) eim_simple_sub_uN((ret)->u, (lhs)->u, (rhs)->u, 4)
#define eim_simple_sub_u512(ret, lhs, rhs) eim_simple_sub_uN((ret)->u, (lhs)->u, (rhs)->u, 8)

void eim_simple_mul_u256(eit_u512_t* ret, const eit_u256_t* lhs, const eit_u256_t* rhs);

void eim_add_mod_p_u256(eit_u256_t* ret, const eit_u256_t* lhs, const eit_u256_t* rhs);
void eim_sub_mod_p_u256(eit_u256_t* ret, const eit_u256_t* lhs, const eit_u256_t* rhs);
void eim_mul_mod_p_u256(eit_u256_t* ret, const eit_u256_t* lhs, const eit_u256_t* rhs);
void eim_pow_mod_p_u256(eit_u256_t* ret, const eit_u256_t* lhs, const eit_u256_t* rhs);

void eim_mod_order_u512(eit_u256_t* ret, const eit_u512_t* val);

#endif /* ED25519_ARBITRARY_MATH_H */