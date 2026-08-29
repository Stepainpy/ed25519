#include <arbmath.h>
#include "out.h"

int main(void) {
    FILE* tvf = NULL;
    eit_u512_t R[1];
    eit_u256_t N[2];
    int rc, count;

    if (!(tvf = fopen("tv/mod_order.txt", "r")))
        error(1, "Couldn't open test vector file");

    for (count = 0; 1; count++) {
        if (read_u512(R, tvf)) break;
        if (read_u256(N + 1, tvf))
            error(1, "Couldn't read correct remainder");

        eim_mod_order_u512(N + 0, R);
        if (eim_cmp_u256(N + 0, N + 1) != 0)
            error(2, "Incorrect remainder");
    }

    printf("[Modulo by order] Check %i test cases\n", count);

    rc = 0;
cleanup:
    if (tvf) fclose(tvf);
    if (rc == 2) {
        fputs("  ", stderr); write_u512(R, stderr); fputc('\n', stderr);
        fputs("  "
            "----------------------------------------------------------------"
            "----------------------------------------------------------------"
            "\n", stderr);
        fputs("X ", stderr); write_u256(N + 0, stderr); fputc('\n', stderr);
        fputs("O ", stderr); write_u256(N + 1, stderr); fputc('\n', stderr);
    }
    return rc;
}