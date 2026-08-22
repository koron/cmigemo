// vim:set ts=8 sts=4 sw=4 tw=0 et:

#include <stdio.h>

int test1(void);
int test_romaji(void);
int test_rxgen(void);

int
main(int argc, char **argv)
{
    int r;
    if ((r = test1()) != 0)
        return r;
    if ((r = test_romaji()) != 0)
        return r;
    if ((r = test_rxgen()) != 0)
        return r;
    // FIXME: Future test cases (group) will be added here.
    return 0;
}
