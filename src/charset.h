// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// charset.h -
//
// Written By:  MURAOKA Taro <koron.kaoriya@gmail.com>

#pragma once

enum {
    CHARSET_NONE = 0,
    CHARSET_CP932 = 1,
    CHARSET_EUCJP = 2,
    CHARSET_UTF8 = 3,
};

// The maximum number of bytes into which a single code point can be expanded.
#define CHARSET_MAX_BYTES 6

typedef int (*charset_proc_char2int)(const unsigned char *, unsigned int *);
typedef int (*charset_proc_int2char)(unsigned int, unsigned char *);
#define CHARSET_PROC_CHAR2INT charset_proc_char2int
#define CHARSET_PROC_INT2CHAR charset_proc_int2char

extern const unsigned char CHARSET_UTF8_LUT[128];

#ifdef __cplusplus
extern "C" {
#endif

int charset_none_int2char(unsigned int in, unsigned char *out);

int charset_cp932_char2int(const unsigned char *in, unsigned int *out);
int charset_cp932_int2char(unsigned int in, unsigned char *out);
int charset_eucjp_char2int(const unsigned char *in, unsigned int *out);
int charset_eucjp_int2char(unsigned int in, unsigned char *out);
int charset_utf8_char2int(const unsigned char *in, unsigned int *out);
int charset_utf8_int2char(unsigned int in, unsigned char *out);

int charset_detect_file(const char *path);
void charset_getproc(int charset, CHARSET_PROC_CHAR2INT *char2int,
        CHARSET_PROC_INT2CHAR *int2char);

#ifdef __cplusplus
}
#endif

static inline int
charset_none_char2int(const unsigned char *in, unsigned int *out)
{
    if (out)
        *out = *in;
    return 1;
}


static inline int
charset_utf8_char2int_inner(const unsigned char *in, unsigned int *out)
{
    if (!(in[0] & 0x80))
        return charset_none_char2int(in, out);

    int len = CHARSET_UTF8_LUT[in[0] - 128];
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

static inline unsigned int
charset_decode(CHARSET_PROC_CHAR2INT proc, const unsigned char **pptr)
{
    unsigned int code = 0;
    *pptr += proc ? proc(*pptr, &code) : charset_utf8_char2int_inner(*pptr, &code);
    return code;
}
