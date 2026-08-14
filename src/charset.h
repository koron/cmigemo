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

typedef int (*charset_proc_char2int)(const unsigned char *, unsigned int *);
typedef int (*charset_proc_int2char)(unsigned int, unsigned char *);
#define CHARSET_PROC_CHAR2INT charset_proc_char2int
#define CHARSET_PROC_INT2CHAR charset_proc_int2char

#ifdef __cplusplus
extern "C" {
#endif

int charset_none_char2int(const unsigned char *in, unsigned int *out);
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

static inline unsigned int
charset_decode(CHARSET_PROC_CHAR2INT proc, const unsigned char **pptr)
{
    unsigned int code = 0;
    int len = proc(*pptr, &code);
    if (len == 0)
        len = charset_none_char2int(*pptr, &code);
    if (code)
        *pptr += len;
    return code;
}
