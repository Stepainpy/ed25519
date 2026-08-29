#include <ed25519/ed25519.h>
#include <random.h>
#include "out.h"

#include <string.h>

static eit_u64_t rol(eit_u64_t n, int s) {
    return n << s | n >> (-s & 63);
}

static eit_u64_t s[4];

static eit_u64_t next(void) {
    eit_u64_t out = rol(s[1] * 5, 7) * 9;
    eit_u64_t t = s[1] << 17;

    s[2] ^= s[0];
    s[3] ^= s[1];
    s[1] ^= s[2];
    s[0] ^= s[3];
    s[2] ^= t;

    s[3] = rol(s[3], 45);

    return out;
}

int main(void) {
    FILE* tvf = NULL;
    ed25519_private_key_t pvt;
    ed25519_public_key_t pubX, pubO;
    ed25519_signature_t signX, signO;
    ed25519_byte_t msg[1024];
    int rc, count; size_t iv;

    if (!(tvf = fopen("tv/verify.txt", "r")))
        error(1, "Couldn't open test vector file");

    eir_generate((void*)s, sizeof s);

    for (count = 0; count < 1024; count++) {
        if (read_buffer(pvt, sizeof pvt, tvf))
            error(1, "Couldn't read private key");
        if (read_buffer(pubO, sizeof pubO, tvf))
            error(1, "Couldn't read public key");

        if (fgetc(tvf) != ':') error(-1, "no delim");
        if (read_buffer(pubX, sizeof pubX, tvf)) error(1, "no dub pub");
        if (fgetc(tvf) != ':') error(-2, "no delim");

        if (read_buffer(msg, count, tvf))
            error(1, "Couldn't read message");
        if (fgetc(tvf) != ':') error(-3, "no delim");

        if (read_buffer(signO, sizeof signO, tvf))
            error(1, "Couldn't read signature");
        if (read_buffer(msg, count, tvf)) error(1, "no dub msg");
        if (fgetc(tvf) != ':') error(-4, "no delim");

        /* valid case */

        if (ed25519_pvt_to_pub(pubX, pvt))
            error(1, "Couldn't create public key");
        if (memcmp(pubX, pubO, sizeof pubO) != 0)
            error(2, "Not equal public keys");

        if (ed25519_sign(msg, count, pvt, signX))
            error(1, "Couldn't create signature");
        if (memcmp(signX, signO, sizeof signO) != 0)
            error(3, "Not equal signatures");

        if (ed25519_verify(msg, count, pubO, signO))
            error(4, "Fail verify");

        /* invalid message */

        if (count > 0) {
            iv = next();
            msg[1023] = msg[iv % count];
            msg[iv % count] = ~msg[iv % count];

            if (!ed25519_verify(msg, count, pubO, signO))
                error(5, "Invalid message, valid signature");

            msg[iv % count] = msg[1023];
        }

        /* invalid public key */

        iv = next();
        pubX[iv % 32] = ~pubX[iv % 32];

        if (!ed25519_verify(msg, count, pubX, signO))
            error(6, "Invalid pubkey, valid signature");

        /* invalid signature */

        iv = next();
        signX[iv % 64] = ~signX[iv % 64];

        if (!ed25519_verify(msg, count, pubO, signX))
            error(7, "Invalid signature");
    }

    printf("[Signing and verifying] Check %i test cases\n", count);

    rc = 0;
cleanup:
    if (tvf) fclose(tvf);
    if (rc < 0) {
        fprintf(stderr, "Delimiter in #%i\n", -rc);
    }

    switch (rc) {
    case 2:
        fputs("  ", stderr); write_buffer(pvt , sizeof pvt , stderr); fputc('\n', stderr);
        fputs("X ", stderr); write_buffer(pubX, sizeof pubX, stderr); fputc('\n', stderr);
        fputs("O ", stderr); write_buffer(pubO, sizeof pubO, stderr); fputc('\n', stderr);
        break;
    case 3:
        fputs("X ", stderr); write_buffer(signX, sizeof signX, stderr); fputc('\n', stderr);
        fputs("O ", stderr); write_buffer(signO, sizeof signO, stderr); fputc('\n', stderr);
        break;
    case 4:
        write_buffer(msg, count, stderr); fputs(" -> ", stderr);
        write_buffer(signO, sizeof signO, stderr); fputc('\n', stderr);
        break;

    case 5:
        fputs("X ", stderr); write_buffer(msg, count, stderr); fputc('\n', stderr); msg[iv % count] = msg[1023];
        fputs("O ", stderr); write_buffer(msg, count, stderr); fputc('\n', stderr);
        fprintf(stderr, "%*s^^\n", (int)(iv % count) * 2, "");
        break;
    case 6:
        fputs("X ", stderr); write_buffer(pubX, sizeof pubX, stderr); fputc('\n', stderr);
        fputs("O ", stderr); write_buffer(pubO, sizeof pubO, stderr); fputc('\n', stderr);
        fprintf(stderr, "%*s^^\n", (int)(iv % 32) * 2, "");
        break;
    case 7:
        fputs("X ", stderr); write_buffer(signX, sizeof signX, stderr); fputc('\n', stderr);
        fputs("O ", stderr); write_buffer(signO, sizeof signO, stderr); fputc('\n', stderr);
        fprintf(stderr, "%*s^^\n", (int)(iv % 64) * 2, "");
        break;
    }

    return rc;
}