#ifndef ED25519_ENCODE_H
#define ED25519_ENCODE_H

#include <types.h>

#define EIE_OKEY 0
#define EIE_FAIL 1

void eie_encode_point(eit_byte_t* dst, const eit_point_t* P);
int  eie_decode_point(eit_point_t* P, const eit_byte_t* src);

#endif /* ED25519_ENCODE_H */