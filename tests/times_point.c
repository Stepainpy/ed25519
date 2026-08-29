#include <arith.h>
#include "out.h"

int main(void) {
    FILE* tvf = NULL;
    eit_u256_t  N[1];
    eit_point_t P[3];
    int rc, count;

    if (!(tvf = fopen("tv/times_point.txt", "r")))
        error(1, "Couldn't open test vector file");

    for (count = 0; count < 1000; count++) {
        if (read_u256(N, tvf)) break;
        if (read_point(P, tvf))
            error(1, "Couldn't read point");
        if (read_point(P + 2, tvf))
            error(1, "Couldn't read correct times product");

        eia_times_point(P + 1, N, P + 0);
        if (!equal_point(P + 1, P + 2))
            error(2, "Incorrect times product");
    }

    printf("[Times points] Check %i test cases\n", count);

    rc = 0;
cleanup:
    if (tvf) fclose(tvf);
    if (rc == 2) {
        fputs("  ", stderr); write_u256 (N, stderr); fputc('\n', stderr);
        fputs("* ", stderr); write_point(P, stderr); fputc('\n', stderr);
        fputs("  "
            "----------------------------------------------------------------"
            "----------------------------------------------------------------"
            "----------------------------------------------------------------"
            "----------------------------------------------------------------"
            "\n", stderr);
        fputs("X ", stderr); write_point(P + 1, stderr); fputc('\n', stderr);
        fputs("O ", stderr); write_point(P + 2, stderr); fputc('\n', stderr);
    }
    return rc;
}