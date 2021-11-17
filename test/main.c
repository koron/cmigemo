/* vim:set ts=8 sts=4 sw=4 tw=0 et: */

#include <stdio.h>

int test1(void);

    int
main(int argc, char** argv)
{
    int r;
    if (r = test1())
        return r;
    return 0;
}
