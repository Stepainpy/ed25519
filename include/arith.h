#ifndef ED25519_ARITHMETIC_H
#define ED25519_ARITHMETIC_H

#include <types.h>

void eia_add_point(eit_point_t* R, const eit_point_t* P, const eit_point_t* Q);

void eia_times_point(eit_point_t* R, const eit_u256_t* n, const eit_point_t* P);

#endif /* ED25519_ARITHMETIC_H */