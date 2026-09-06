#pragma once

#include <stdlib.h>

typedef struct ioreader ioreader;

#ifdef __cplusplus
extern "C" {
#endif

ioreader *ioreader_open(const char *filename, const char *mode);
void ioreader_close(ioreader *r);

size_t ioreader_read(void *ptr, size_t size, size_t nmemb, ioreader *r);
int ioreader_eof(ioreader *r);
int ioreader_error(ioreader *r);

#ifdef __cplusplus
}
#endif
