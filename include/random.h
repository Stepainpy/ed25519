#ifndef ED25519_RANDOM_H
#define ED25519_RANDOM_H

#include <types.h>

#include <stddef.h>

size_t eir_generate(eit_byte_t* dest, size_t count);

#endif /* ED25519_RANDOM_H */