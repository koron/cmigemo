/* vim:set ts=8 sts=4 sw=4 tw=0 et: */

#include <stdio.h>
#include <string.h>
#include "migemo.h"

    static int
assert_query(migemo *p, const char *q, const char *ex)
{
    char    *r;

    r = migemo_query(p, q);
    if (strcmp(r, ex) != 0)
    {
        printf("Failed: query \"%s\" generate \"%s\" (expected \"%s\")\n",
                q, r, ex);
        migemo_release(p, r);
        return 1;
    }
    migemo_release(p, r);
    return 0;
}

    static int
test_all(migemo *p)
{
    if (assert_query(p, "ak",
                "([明悪秋朱紅赤]|あ([こけくきか]|っ[こけくきか])|ak)") != 0)
        return 1;
    if (assert_query(p, "n", "[んのねぬになn]") != 0)
        return 1;
    /* FIXME: add tests */

    return 0;
}

    int
test1(void)
{
    int     r;
    migemo  *p;

    p = migemo_open("test1/migemo-dict");
    if (p == NULL)
    {
        printf("Failed: can't create migemo object and get its pointer\n");
        return 1;
    }
    r = migemo_load(p, MIGEMO_DICTID_ROMA2HIRA, "../dict2/roma2hira.dat");
    if (r != MIGEMO_DICTID_ROMA2HIRA)
    {
        printf("Failed: can't load \"../dict2/roma2hira.dat (%d)\n",
                MIGEMO_DICTID_ROMA2HIRA);
        return r;
    }

    r = test_all(p);
    migemo_close(p);
    return r;
}
