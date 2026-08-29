#include <sha512.h>
#include "out.h"

#include <string.h>

static int read_hash(eih_hash_t* hash, FILE* file) {
    return read_buffer(hash->byte, sizeof hash->byte, file);
}

static int write_hash(const eih_hash_t* hash, FILE* file) {
    return write_buffer(hash->byte, sizeof hash->byte, file);
}

int main(void) {
    eih_context_t ctx[1];
    FILE* tvf = NULL;
    eit_byte_t B[32];
    eih_hash_t H[2];
    int rc, count;

    if (!(tvf = fopen("tv/sha512.txt", "r")))
        error(1, "Couldn't open test vector file");

    for (count = 0; count < 1000000; count++) {
        if (read_buffer(B, sizeof B, tvf)) break;
        if (read_hash(H + 1, tvf))
            error(1, "Couldn't read correct hash");

        eih_init(ctx);
        eih_next(ctx, B, sizeof B);
        eih_halt(ctx, H);
        if (memcmp(H[0].byte, H[1].byte, sizeof H->byte) != 0)
            error(2, "Incorrect hash");
    }

    printf("[SHA512 hashing] Check %i test cases\n", count);

    rc = 0;
cleanup:
    if (tvf) fclose(tvf);
    if (rc == 2) {
        fputs("> ", stderr); write_buffer(B, sizeof B, stderr); fputc('\n', stderr);
        fputs("  "
            "----------------------------------------------------------------"
            "----------------------------------------------------------------"
            "\n", stderr);
        fputs("X ", stderr); write_hash(H + 0, stderr); fputc('\n', stderr);
        fputs("O ", stderr); write_hash(H + 1, stderr); fputc('\n', stderr);
    }
    return rc;
}