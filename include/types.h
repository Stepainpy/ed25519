#ifndef ED25519_TYPES_H
#define ED25519_TYPES_H

#include <stdint.h>

#define EIT_U64_C(lit) UINT64_C(lit)

typedef uint8_t eit_byte_t;

typedef uint32_t eit_u32_t;
typedef uint64_t eit_u64_t;
typedef  int64_t eit_i64_t;

typedef struct { eit_u64_t u[2]; } eit_u128_t;
typedef struct { eit_u64_t u[4]; } eit_u256_t;
typedef struct { eit_u64_t u[8]; } eit_u512_t;

typedef struct { eit_u256_t x, y, z, t; } eit_point_t;

#endif /* ED25519_TYPES_H */