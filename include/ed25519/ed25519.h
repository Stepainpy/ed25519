#ifndef ED25519_H
#define ED25519_H

#include <stddef.h>

#define ED25519_PRIVATE_KEY_SIZE 32
#define ED25519_PUBLIC_KEY_SIZE 32
#define ED25519_SIGNATURE_SIZE 64

enum {
    ED25519_OKEY = 0,
    ED25519_FAIL = 1
};

typedef unsigned char ed25519_byte_t;

typedef ed25519_byte_t ed25519_private_key_t[ED25519_PRIVATE_KEY_SIZE];
typedef ed25519_byte_t ed25519_public_key_t[ED25519_PUBLIC_KEY_SIZE];
typedef ed25519_byte_t ed25519_signature_t[ED25519_SIGNATURE_SIZE];

int ed25519_pvt_to_pub(ed25519_public_key_t pub_key, const ed25519_private_key_t pvt_key);
int ed25519_keygen(ed25519_private_key_t pvt_key, ed25519_public_key_t pub_key);

int ed25519_sign  (const void* data, size_t size,
    const ed25519_private_key_t pvt_key,       ed25519_signature_t sign);
int ed25519_verify(const void* data, size_t size,
    const ed25519_public_key_t  pub_key, const ed25519_signature_t sign);

#endif /* ED25519_H */