/* vim:set ts=8 sts=4 sw=4 tw=0: */
/*
 * romaji_main.c -
 *
 * Written By:  Muraoka Taro <koron@tka.att.ne.jp>
 * Last Change: 18-Oct-2003.
 */
/*
 * gcc -o romaji romaji_main.c ../romaji.c ../wordbuf.c
 */

#include <stdio.h>
#include <string.h>
#include "../romaji.h"

#ifndef DICT_ROMA2HIRA
# define DICT_ROMA2HIRA	"../dict/roma2hira.dat"
#endif
#ifndef DICT_HIRA2KATA
# define DICT_HIRA2KATA	"../dict/hira2kata.dat"
#endif

    void
query_loop(romaji* object, romaji* hira2kata)
{
    char buf[256], *ans;

    while (1)
    {
	printf("QUERY: ");
	if (!fgets(buf, sizeof(buf), stdin))
	{
	    printf("\n");
	    break;
	}
	/* â¸çsÇNULï∂éöÇ…íuÇ´ä∑Ç¶ÇÈ */
	if ((ans = strchr(buf, '\n')) != NULL)
	    *ans = '\0';

	{
	    unsigned char *stop;
	    unsigned char *hira;
	    if (hira = romaji_convert(object, buf, &stop))
	    {
		unsigned char* kata;

		printf("  hira=%s, stop=%s\n", hira, stop);
		if (kata = romaji_convert(hira2kata, hira, &stop))
		{
		    printf("  kata=%s, stop=%s\n", kata, stop);
		    romaji_release(hira2kata, kata);
		}
		fflush(stdout);
		romaji_release(object, hira);
	    }

	}
    }
}

    int
main(int argc, char** argv)
{
    romaji *object, *hira2kata;

    object = romaji_open();
    hira2kata = romaji_open();

    if (object && hira2kata)
    {
	int retval = 0;

	retval = romaji_load(object, DICT_ROMA2HIRA);
	printf("romaji_load(%s)=%d\n", DICT_ROMA2HIRA, retval);
	retval = romaji_load(hira2kata, DICT_HIRA2KATA);
	printf("romaji_load(%s)=%d\n", DICT_HIRA2KATA, retval);
	query_loop(object, hira2kata);
    }

    if (hira2kata)
	romaji_close(hira2kata);
    if (object)
	romaji_close(object);

    return 0;
}
