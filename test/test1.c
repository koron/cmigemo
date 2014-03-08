/* vim:set ts=8 sts=4 sw=4 tw=0 et: */

#include <stdio.h>
#include <migemo.h>

    static int
test_all(migemo *p)
{
    /* TODO: add tests */
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
        printf("Failed: migemo_open(\"test1/migemo-dict\") return NULL\n");
        return 1;
    }
    r = test_all(p);
    migemo_close(p);
    return r;
}
