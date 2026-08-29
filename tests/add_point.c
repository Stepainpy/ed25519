#include <arith.h>
#include "out.h"

int main(void) {
    FILE* tvf = NULL;
    eit_point_t P[4];
    int rc, count;

    if (!(tvf = fopen("tv/add_point.txt", "r")))
        error(1, "Couldn't open test vector file");

    for (count = 0; 1; count++) {
        if (read_point(P + 0, tvf)) break;
        if (read_point(P + 1, tvf))
            error(1, "Couldn't read second point");
        if (read_point(P + 3, tvf))
            error(1, "Couldn't read correct sum");

        eia_add_point(P + 2, P + 0, P + 1);
        if (!equal_point(P + 2, P + 3))
            error(2, "Incorrect sum");
    }

    printf("[Add points] Check %i test cases\n", count);

    rc = 0;
cleanup:
    if (tvf) fclose(tvf);
    if (rc == 2) {
        fputs("  ", stderr); write_point(P + 0, stderr); fputc('\n', stderr);
        fputs("+ ", stderr); write_point(P + 1, stderr); fputc('\n', stderr);
        fputs("  "
            "----------------------------------------------------------------"
            "----------------------------------------------------------------"
            "----------------------------------------------------------------"
            "----------------------------------------------------------------"
            "\n", stderr);
        fputs("X ", stderr); write_point(P + 2, stderr); fputc('\n', stderr);
        fputs("O ", stderr); write_point(P + 3, stderr); fputc('\n', stderr);
    }
    return rc;
}