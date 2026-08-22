// vim:set ts=8 sts=4 sw=4 tw=0 et:

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "romaji.h"
#include "wordlist.h"

#include "test_common.h"

static uint32_t
count_list(wordlist *list)
{
    uint32_t count = 0;
    for (; list; list = list->next)
        count++;
    return count;
}

static int
check_all(romaji *rj, const unsigned char *src, const unsigned char **want)
{
    int result = 1;
    wordlist *list = romaji_convert_all(rj, src);

    uint32_t got_count = count_list(list);
    uint32_t want_count = 0;
    for (const unsigned char **p = want; *p; p++)
        want_count++;

    if (got_count != want_count)
    {
        printf("count mismatch: want=%d got=%d src=%s\n", want_count, got_count,
                src);
        goto END;
    }
    if (got_count == 0)
    {
        result = 0;
        goto END;
    }

    wordlist *wl = list;
    for (uint32_t i = 0; i < got_count; i++)
    {
        if (strcmp(want[i], wl->ptr))
        {
            printf("#%d mismatch: want=%s got=%s src=%s\n", i, want[i], wl->ptr,
                    src);
            goto END;
        }
        wl = wl->next;
    }

    result = 0;
END:
    if (list && result != 0)
    {
        printf("got list for src=%s\n", src);
        int i = 0;
        for (wordlist *curr = list; curr; curr = curr->next)
            printf("  #%d %s\n", i++, curr->ptr);
    }
    if (list)
        wordlist_destroy(list);
    return result;
}

static int
check_one(romaji *rj, const unsigned char *src, const unsigned char *want)
{
    return check_all(rj, src, (const unsigned char *[]){ want, NULL });
}

static int
test_roma2hira(void)
{
    romaji *rj = romaji_open();
    int has_error = 1;
    if (!rj)
    {
        printf("Failed: romaji_open() returned NULL\n");
        goto END;
    }

    int r;
    r = romaji_load(rj, TEST_DICTDIR_ROMA2HIRA "/roma2hira.dat", NULL);
    if (r != 0)
    {
        printf("Failed: romaji_load() returned non zero: %d", r);
        goto END;
    }

    r = check_all(rj, "a", (const unsigned char *[]){"あ", NULL});
    if (r != 0)
        goto END;

    r = check_all(rj, "aki", (const unsigned char *[]){"あき", NULL});
    if (r != 0)
        goto END;

    r = check_all(rj, "k",
            (const unsigned char *[]){"か", "け", "き", "っ", "こ", "く",
                    "くぁ", "きゃ", "きぇ", "きぃ", "きょ", "きゅ", NULL});
    if (r != 0)
        goto END;

    r = check_all(rj, "nya", (const unsigned char *[]){"にゃ", NULL});
    if (r != 0)
        goto END;

    r = check_all(rj, "ny",
            (const unsigned char *[]){
                    "にゃ", "にぇ", "にぃ", "にょ", "にゅ", NULL});
    if (r != 0)
        goto END;

    r = check_all(rj, "n",
            (const unsigned char *[]){"ん", "な", "ね", "に", "の", "ぬ",
                    "にゃ", "にぇ", "にぃ", "にょ", "にゅ", NULL});
    if (r != 0)
        goto END;

    has_error = 0; // Passed all cases.
END:
    if (rj)
        romaji_close(rj);
    return has_error;
}

static int
test_hira2kana(void)
{
    romaji *rj = romaji_open();
    int has_error = 1;
    if (!rj)
    {
        printf("Failed: romaji_open() returned NULL\n");
        goto END;
    }

    int r;
    r = romaji_load(rj, TEST_DICTDIR_ROMA2HIRA "/hira2kata.dat", NULL);
    if (r != 0)
    {
        printf("Failed: romaji_load() returned non zero: %d", r);
        goto END;
    }

    r = check_one(rj, "あ", "ア");
    if (r != 0)
        goto END;

    r = check_one(rj, "にゃ", "ニャ");
    if (r != 0)
        goto END;

    r = check_one(rj, "きゃんでぃ", "キャンディ");
    if (r != 0)
        goto END;

    has_error = 0; // Passed all cases.
END:
    if (rj)
        romaji_close(rj);
    return has_error;
}

int
test_romaji(void)
{
    int r;
    if ((r = test_roma2hira()) != 0)
        return r;
    if ((r = test_hira2kana()) != 0)
        return r;
    return 0;
}
