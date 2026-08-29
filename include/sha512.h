#ifndef ED25519_SHA_512_H
#define ED25519_SHA_512_H

#include <types.h>

#include <stddef.h>

typedef struct {
    eit_u64_t hash[8], len[2];
    eit_byte_t buffer[128], buflen;
} eih_context_t;

typedef struct {
    eit_byte_t byte[64];
} eih_hash_t;

void eih_init(eih_context_t* ctx);
void eih_next(eih_context_t* ctx, const void* data, size_t count);
void eih_halt(eih_context_t* ctx, eih_hash_t* hash);

#endif /* ED25519_SHA_512_H */