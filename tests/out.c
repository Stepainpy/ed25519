#include "out.h"
#include <arbmath.h>

int read_buffer(eit_byte_t* b, size_t n, FILE* file) {
    size_t i, c; unsigned x = 0;
    for (i = c = 0; i < n; i++)
        c += fscanf(file, "%2x", &x), b[i] = x;
    return c != n;
}

int write_buffer(const eit_byte_t* b, size_t n, FILE* file) {
    size_t i, c; for (i = c = 0; i < n; i++)
        c += fprintf(file, "%02x", (unsigned)b[i]);
    return c != 2 * n;
}

int read_u128(eit_u128_t* x, FILE* file) {
    int n = fscanf(file, "%16lx""%16lx", x->u + 1, x->u);
    return n != 2;
}

int read_u256(eit_u256_t* x, FILE* file) {
    int n = fscanf(file,
        "%16lx""%16lx""%16lx""%16lx",
        x->u + 3, x->u + 2, x->u + 1, x->u
    );
    return n != 4;
}

int read_u512(eit_u512_t* x, FILE* file) {
    int n = fscanf(file,
        "%16lx""%16lx""%16lx""%16lx"
        "%16lx""%16lx""%16lx""%16lx",
        x->u + 7, x->u + 6, x->u + 5, x->u + 4,
        x->u + 3, x->u + 2, x->u + 1, x->u + 0
    );
    return n != 8;
}

int write_u128(const eit_u128_t* x, FILE* file) {
    int n = fprintf(file, "%016lx""%016lx", x->u[1], x->u[0]);
    return n != 32;
}

int write_u256(const eit_u256_t* x, FILE* file) {
    int n = fprintf(file,
        "%016lx""%016lx""%016lx""%016lx",
        x->u[3], x->u[2], x->u[1], x->u[0]
    );
    return n != 64;
}

int write_u512(const eit_u512_t* x, FILE* file) {
    int n = fprintf(file,
        "%016lx""%016lx""%016lx""%016lx"
        "%016lx""%016lx""%016lx""%016lx",
        x->u[7], x->u[6], x->u[5], x->u[4],
        x->u[3], x->u[2], x->u[1], x->u[0]
    );
    return n != 128;
}

int read_point(eit_point_t* P, FILE* file) {
    if (read_u256(&P->x, file)) return 1;
    if (read_u256(&P->y, file)) return 1;
    if (read_u256(&P->z, file)) return 1;
    if (read_u256(&P->t, file)) return 1;
    return 0;
}

int write_point(const eit_point_t* P, FILE* file) {
    if (write_u256(&P->x, file)) return 1;
    if (write_u256(&P->y, file)) return 1;
    if (write_u256(&P->z, file)) return 1;
    if (write_u256(&P->t, file)) return 1;
    return 0;
}

int equal_point(const eit_point_t* P, const eit_point_t* Q) {
    if (eim_cmp_u256(&P->x, &Q->x) != 0) return 0;
    if (eim_cmp_u256(&P->y, &Q->y) != 0) return 0;
    if (eim_cmp_u256(&P->z, &Q->z) != 0) return 0;
    if (eim_cmp_u256(&P->t, &Q->t) != 0) return 0;
    return 1;
}
