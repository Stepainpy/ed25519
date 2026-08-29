#include <arbmath.h>
#include "out.h"

int main(void) {
    FILE* tvf = NULL;
    eit_u256_t N[4];
    int rc, count;

    if (!(tvf = fopen("tv/sub_mod_p.txt", "r")))
        error(1, "Couldn't open test vector file");

    for (count = 0; 1; count++) {
        if (read_u256(N + 0, tvf)) break;
        if (read_u256(N + 1, tvf))
            error(1, "Couldn't read second integer");
        if (read_u256(N + 3, tvf))
            error(1, "Couldn't read correct sum");

        eim_sub_mod_p_u256(N + 2, N + 0, N + 1);
        if (eim_cmp_u256(N + 2, N + 3) != 0)
            error(2, "Incorrect difference");
    }

    printf("[Sub by modulo p] Check %i test cases\n", count);

    rc = 0;
cleanup:
    if (tvf) fclose(tvf);
    if (rc == 2) {
        fputs("  ", stderr); write_u256(N + 0, stderr); fputc('\n', stderr);
        fputs("- ", stderr); write_u256(N + 1, stderr); fputc('\n', stderr);
        fputs("  ----------------------------------------------------------------\n", stderr);
        fputs("X ", stderr); write_u256(N + 2, stderr); fputc('\n', stderr);
        fputs("O ", stderr); write_u256(N + 3, stderr); fputc('\n', stderr);
    }
    return rc;
}