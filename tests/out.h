#ifndef ED25519_TESTS_OUTPUT_H
#define ED25519_TESTS_OUTPUT_H

#include <types.h>

#include <stdio.h>

#define error(ret, msg) do { \
    fputs("[ERROR] "msg"\n", stderr); \
    rc = ret; goto cleanup; \
} while (0)

int read_buffer(eit_byte_t* b, size_t n, FILE* file);
int write_buffer(const eit_byte_t* b, size_t n, FILE* file);

int read_u128(eit_u128_t* x, FILE* file);
int read_u256(eit_u256_t* x, FILE* file);
int read_u512(eit_u512_t* x, FILE* file);

int write_u128(const eit_u128_t* x, FILE* file);
int write_u256(const eit_u256_t* x, FILE* file);
int write_u512(const eit_u512_t* x, FILE* file);

int read_point(eit_point_t* P, FILE* file);
int write_point(const eit_point_t* P, FILE* file);

int equal_point(const eit_point_t* P, const eit_point_t* Q);

#endif /* ED25519_TESTS_OUTPUT_H */