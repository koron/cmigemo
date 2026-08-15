// vim:set ts=8 sts=4 sw=4 tw=0 et:

#include <stdio.h>
#include <string.h>

#include "migemo.h"
#include "rxgen.h"

int
test2(void)
{
    rxgen *rx = rxgen_open();
    if (!rx)
    {
        printf("Failed: rxgen_open() returned NULL\n");
        return 1;
    }

    // Test rxgen_set_escape_chars with NULL (default set: \.*+^$/)
    rxgen_set_escape_chars(rx, NULL);
    rxgen_add(rx, (const unsigned char *)"a+b.c*d^e$f/g\\h?i");
    unsigned char *pat = rxgen_generate(rx);
    if (!pat)
    {
        printf("Failed: rxgen_generate() returned NULL\n");
        rxgen_close(rx);
        return 1;
    }
    const char *expected_default = "a\\+b\\.c\\*d\\^e\\$f\\/g\\\\h?i";
    if (strcmp((const char *)pat, expected_default) != 0)
    {
        printf("Failed default rxgen escape: got \"%s\", expected \"%s\"\n", pat, expected_default);
        rxgen_release(rx, pat);
        rxgen_close(rx);
        return 1;
    }
    rxgen_release(rx, pat);

    // Test rxgen_set_escape_chars with custom string including out-of-range chars
    rxgen_reset(rx);
    // Escape '?' and '+' (plus ASCII out-of-range chars 0x01, 0xFF)
    rxgen_set_escape_chars(rx, (const unsigned char *)"\x01?+\xFF");
    rxgen_add(rx, (const unsigned char *)"a+b.c?d");
    pat = rxgen_generate(rx);
    const char *expected_custom = "a\\+b.c\\?d";
    if (strcmp((const char *)pat, expected_custom) != 0)
    {
        printf("Failed custom rxgen escape: got \"%s\", expected \"%s\"\n", pat, expected_custom);
        rxgen_release(rx, pat);
        rxgen_close(rx);
        return 1;
    }
    rxgen_release(rx, pat);

    // Test rxgen_set_escape_chars with empty string "" (no escaping)
    rxgen_reset(rx);
    rxgen_set_escape_chars(rx, (const unsigned char *)"");
    rxgen_add(rx, (const unsigned char *)"a+b.c\\d");
    pat = rxgen_generate(rx);
    const char *expected_empty = "a+b.c\\d";
    if (strcmp((const char *)pat, expected_empty) != 0)
    {
        printf("Failed empty rxgen escape: got \"%s\", expected \"%s\"\n", pat, expected_empty);
        rxgen_release(rx, pat);
        rxgen_close(rx);
        return 1;
    }
    rxgen_release(rx, pat);
    rxgen_close(rx);

    // Test migemo_set_escape_chars API
    if (migemo_set_escape_chars(NULL, (const unsigned char *)"+") == 0)
    {
        printf("Failed: migemo_set_escape_chars(NULL, ...) should return non-zero\n");
        return 1;
    }

    migemo *mo = migemo_open(NULL);
    if (!mo)
    {
        printf("Failed: migemo_open(NULL) returned NULL\n");
        return 1;
    }

    if (migemo_set_escape_chars(mo, (const unsigned char *)"+?") != 0)
    {
        printf("Failed: migemo_set_escape_chars(mo, \"+?\") returned non-zero\n");
        migemo_close(mo);
        return 1;
    }

    unsigned char *qres = migemo_query(mo, (const unsigned char *)"a+b?c.d");
    if (!qres)
    {
        printf("Failed: migemo_query returned NULL\n");
        migemo_close(mo);
        return 1;
    }
    const char *expected_migemo = "a\\+b\\?c.d";
    if (strcmp((const char *)qres, expected_migemo) != 0)
    {
        printf("Failed migemo custom escape query: got \"%s\", expected \"%s\"\n", qres, expected_migemo);
        migemo_release(mo, qres);
        migemo_close(mo);
        return 1;
    }
    migemo_release(mo, qres);
    migemo_close(mo);

    return 0;
}
