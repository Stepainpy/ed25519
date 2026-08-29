#include <arbmath.h>
#include "out.h"

int main(void) {
    FILE* tvf = NULL;
    int rc, count;
    eit_u256_t a[1], b[1];
    eit_u512_t c[1], t[1];

    if (!(tvf = fopen("tv/mul_256to512.txt", "r")))
        error(1, "Couldn't open test vector file");

    for (count = 0; 1; count++) {
        if (read_u256(a, tvf)) break;
        if (read_u256(b, tvf))
            error(1, "Couldn't read second integer");
        if (read_u512(t, tvf))
            error(1, "Couldn't read correct product");

        eim_simple_mul_u256(c, a, b);
        if (eim_cmp_u512(c, t) != 0)
            error(2, "Incorrect product");
    }

    printf("[Mul 256 -> 512] Check %i test cases\n", count);

    rc = 0;
cleanup:
    if (tvf) fclose(tvf);
    if (rc == 2) {
        fputs("                                                                  ", stderr); write_u256(a, stderr); fputc('\n', stderr);
        fputs("*                                                                 ", stderr); write_u256(b, stderr); fputc('\n', stderr);
        fputs("  ----------------------------------------------------------------\n", stderr);
        fputs("X ", stderr); write_u512(c, stderr); fputc('\n', stderr);
        fputs("O ", stderr); write_u512(t, stderr); fputc('\n', stderr);
    }
    return rc;
}