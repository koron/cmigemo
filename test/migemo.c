// vim:set ts=8 sts=4 sw=4 tw=0 et:

#include <stdio.h>
#include <string.h>

#include "migemo.h"
#include "test_common.h"

#ifndef TEST_DICTDIR_MIGEMO
# define TEST_DICTDIR_MIGEMO "test1"
#endif

static int
assert_query(migemo *p, const char *q, const char *ex)
{
    char *r;

    r = migemo_query(p, q);
    if (strcmp(r, ex) != 0)
    {
        printf("Failed: query \"%s\" generate \"%s\" (expected \"%s\")\n", q, r,
                ex);
        migemo_release(p, r);
        return 1;
    }
    migemo_release(p, r);
    return 0;
}

static int
test_all(migemo *p)
{
    if (assert_query(p, "ak", "([悪明朱秋紅赤]|ak|あ[かきくけこっ])") != 0)
        return 1;
    if (assert_query(p, "n", "[nなにぬねのん]") != 0)
        return 1;
    // FIXME: add tests

    return 0;
}

int
main(int argc, char **argv)
{
    int r;
    migemo *p;

    const char *ver = migemo_version();
    if (ver == NULL
            || strcmp(ver, MIGEMO_VERSION MIGEMO_VERSION_PRERELEASE) != 0)
    {
        printf("Failed: migemo_version() returned \"%s\" (expected \"%s\")\n",
                ver ? ver : "(null)", MIGEMO_VERSION MIGEMO_VERSION_PRERELEASE);
        return 1;
    }

    p = migemo_open(TEST_DICTDIR_MIGEMO "/migemo-dict");
    if (p == NULL)
    {
        printf("Failed: can't create migemo object and get its pointer\n");
        return 1;
    }
    r = migemo_load(p, MIGEMO_DICTID_ROMA2HIRA,
            TEST_DICTDIR_ROMA2HIRA "/roma2hira.dat");
    if (r != MIGEMO_DICTID_ROMA2HIRA)
    {
        printf("Failed: can't load \"%s\" (dict_id: %d)\n",
                TEST_DICTDIR_ROMA2HIRA "/roma2hira.dat",
                MIGEMO_DICTID_ROMA2HIRA);
        return r;
    }

    if (test_all(p))
    {
        printf("Failed: mtree\n");
        return 1;
    }

    if (migemo_switch_sdict(p, MIGEMO_SDICT_RELEASE_MDICT) == 0)
    {
        printf("Failed: migemo_switch_sdict\n");
        return 1;
    }
    if (test_all(p))
    {
        printf("Failed: sdict\n");
        return 1;
    }

    migemo_close(p);
    return 0;
}
