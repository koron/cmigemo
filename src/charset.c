// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// charset.c -
//
// Written By:  MURAOKA Taro <koron.kaoriya@gmail.com>

#include "common.h"

#define BUFLEN_DETECT 4096

#include <limits.h>
#include <stdio.h>

#include "charset.h"

int
charset_none_char2int(const unsigned char *in, unsigned int *out)
{
    if (out)
        *out = *in;
    return 1;
}

int
charset_none_int2char(unsigned int in, unsigned char *out)
{
    if (out)
        *out = (unsigned char)in;
    return 1;
}

int
charset_cp932_char2int(const unsigned char *in, unsigned int *out)
{
    unsigned char c0 = in[0];
    unsigned char c1 = c0 ? in[1] : 0;
    if (((c0 >= 0x81 && c0 <= 0x9f) || (c0 >= 0xe0 && c0 <= 0xf0))
            && (c1 >= 0x40 && c1 <= 0xfc && c1 != 0x7f))
    {
        if (out)
            *out = (unsigned int)c0 << 8 | (unsigned int)c1;
        return 2;
    }
    if (out)
        *out = c0;
    return 1;
}

int
charset_cp932_int2char(unsigned int in, unsigned char *out)
{
    if (in >= 0x100)
    {
        if (out)
        {
            out[0] = (unsigned char)((in >> 8) & 0xFF);
            out[1] = (unsigned char)(in & 0xFF);
        }
        return 2;
    }
    if (out)
        out[0] = (unsigned char)(in & 0xFF);
    return 1;
}

#define IS_EUC_RANGE(c) (0xa1 <= (c) && (c) <= 0xfe)

int
charset_eucjp_char2int(const unsigned char *in, unsigned int *out)
{
    unsigned char c0 = in[0];
    unsigned char c1 = c0 ? in[1] : 0;
    if ((c0 == 0x8e && c1 >= 0xa0 && c1 <= 0xdf)
            || (IS_EUC_RANGE(c0) && IS_EUC_RANGE(c1)))
    {
        if (out)
            *out = (unsigned int)c0 << 8 | (unsigned int)c1;
        return 2;
    }
    if (out)
        *out = c0;
    return 1;
}

int
charset_eucjp_int2char(unsigned int in, unsigned char *out)
{
    // Same as CP932, but separated to support JISX0213 in the future
    if (in >= 0x100)
    {
        if (out)
        {
            out[0] = (unsigned char)((in >> 8) & 0xFF);
            out[1] = (unsigned char)(in & 0xFF);
        }
        return 2;
    }
    if (out)
        out[0] = (unsigned char)(in & 0xFF);
    return 1;
}

int
charset_utf8_char2int(const unsigned char *in, unsigned int *out)
{
    if (!(in[0] & 0x80))
        return charset_none_char2int(in, out);

    // Use LUT to determine number of continuation bytes in UTF-8.
    // clang-format off
    static const unsigned char UTF8_LUT[128] = {
            // 0x80 - 0xBF: invalid sequence (continuation byte without leader)
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            // 0xC0 - 0xC1: invalid (overlong encoding)
            0, 0,
            // 0xC2 - 0xDF (110xxxxx): 2 bytes character
                  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
            2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
            // 0xE0 - 0xEF (1110xxxx): 3 bytes character
            3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
            // 0xF0 - 0xF7 (11110xxx): 4 bytes character
            4, 4, 4, 4, 4,
            // 0xF5 - 0xF7: invalid (exceeding Unicode max value of U+10FFFF)
                           0, 0, 0,
            // 0xF8 - 0xFF (11111xxx or greater): invalid values.
                                    0, 0, 0, 0, 0, 0, 0, 0,
    };
    // clang-format on
    int len = UTF8_LUT[in[0] - 128];
    if (!len)
        // Output an invalid byte as-is.
        return charset_none_char2int(in, out);

    unsigned int code = in[0] & (0xff >> len);
    for (int i = 1; i < len; ++i)
    {
        // check invalid byte.
        if ((in[i] & 0xc0) != 0x80)
            // Output an invalid byte as-is.
            return charset_none_char2int(in, out);
        code <<= 6;
        code += in[i] & 0x3f;
    }
    if (out)
        *out = code;
    return len;
}

int
charset_utf8_int2char(unsigned int in, unsigned char *out)
{
    if (in < 0x80)
    {
        if (out)
            out[0] = (unsigned char)(in & 0xFF);
        return 1;
    }
    if (in < 0x800)
    {
        if (out)
        {
            out[0] = 0xc0 + (in >> 6);
            out[1] = 0x80 + ((in >> 0) & 0x3f);
        }
        return 2;
    }
    if (in < 0x10000)
    {
        if (out)
        {
            out[0] = 0xe0 + (in >> 12);
            out[1] = 0x80 + ((in >> 6) & 0x3f);
            out[2] = 0x80 + ((in >> 0) & 0x3f);
        }
        return 3;
    }
    if (in < 0x200000)
    {
        if (out)
        {
            out[0] = 0xf0 + (in >> 18);
            out[1] = 0x80 + ((in >> 12) & 0x3f);
            out[2] = 0x80 + ((in >> 6) & 0x3f);
            out[3] = 0x80 + ((in >> 0) & 0x3f);
        }
        return 4;
    }
    // RFC 3629 specifies that a single UTF-8 character consists of up to 4
    // bytes, so we adhere to that.
    return 0;
}

static int
charset_detect_buf(const unsigned char *buf, int len)
{
    int sjis = 0, smode = 0;
    int euc = 0, emode = 0, eflag = 0;
    int utf8 = 0, umode = 0, ufailed = 0;
    int i;
    for (i = 0; i < len; ++i)
    {
        unsigned char c = buf[i];
        // Check if it is SJIS
        if (smode)
        {
            if ((0x40 <= c && c <= 0x7e) || (0x80 <= c && c <= 0xfc))
                ++sjis;
            smode = 0;
        }
        else if ((0x81 <= c && c <= 0x9f) || (0xe0 <= c && c <= 0xf0))
            smode = 1;
        // Check if it is EUC
        eflag = 0xa1 <= c && c <= 0xfe;
        if (emode)
        {
            if (eflag)
                ++euc;
            emode = 0;
        }
        else if (eflag)
            emode = 1;
        // Check if it is UTF8
        if (!ufailed)
        {
            if (umode < 1)
            {
                if ((c & 0x80) != 0)
                {
                    if ((c & 0xe0) == 0xc0)
                        umode = 1;
                    else if ((c & 0xf0) == 0xe0)
                        umode = 2;
                    else if ((c & 0xf8) == 0xf0)
                        umode = 3;
                    else if ((c & 0xfc) == 0xf8)
                        umode = 4;
                    else if ((c & 0xfe) == 0xfc)
                        umode = 5;
                    else
                    {
                        ufailed = 1;
                        --utf8;
                    }
                }
            }
            else
            {
                if ((c & 0xc0) == 0x80)
                {
                    ++utf8;
                    --umode;
                }
                else
                {
                    --utf8;
                    umode = 0;
                    ufailed = 1;
                }
            }
            if (utf8 < 0)
                utf8 = 0;
        }
    }
    // Finally, return the encoding with the highest score
    if (euc > sjis && euc > utf8)
        return CHARSET_EUCJP;
    else if (!ufailed && utf8 > euc && utf8 > sjis)
        return CHARSET_UTF8;
    else if (sjis > euc && sjis > utf8)
        return CHARSET_CP932;
    else
        return CHARSET_NONE;
}

// character returns a character set encoder/decoder. Even for invalid
// character sets, it returns a primitive encoder and decoder that function on
// a single-byte basis.
void
charset_getproc(int charset, CHARSET_PROC_CHAR2INT *char2int,
        CHARSET_PROC_INT2CHAR *int2char)
{
    CHARSET_PROC_CHAR2INT c2i = NULL;
    CHARSET_PROC_INT2CHAR i2c = NULL;
    switch (charset)
    {
        case CHARSET_CP932:
            c2i = charset_cp932_char2int;
            i2c = charset_cp932_int2char;
            break;
        case CHARSET_EUCJP:
            c2i = charset_eucjp_char2int;
            i2c = charset_eucjp_int2char;
            break;
        case CHARSET_UTF8:
            c2i = charset_utf8_char2int;
            i2c = charset_utf8_int2char;
            break;
        default:
            c2i = charset_none_char2int;
            i2c = charset_none_int2char;
            break;
    }
    if (char2int)
        *char2int = c2i;
    if (int2char)
        *int2char = i2c;
}

int
charset_detect_file(const char *path)
{
    int charset = CHARSET_NONE;
    FILE *fp;
    if ((fp = fopen(path, "rt")) != NULL)
    {
        unsigned char buf[BUFLEN_DETECT];
        size_t len = fread(buf, sizeof(buf[0]), sizeof(buf), fp);
        fclose(fp);
        if (len > 0 && len <= INT_MAX)
            charset = charset_detect_buf(buf, (int)len);
    }
    return charset;
}
