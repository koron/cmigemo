// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// filename.h - Operate filename.
//
// Written by:  Muraoka Taro  <koron.kaoriya@gmail.com>

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

size_t filename_directory(char *dir, size_t cap, const char *path);
size_t filename_join(char *out, size_t cap, const char *dir, const char *file);

#ifdef __cplusplus
}
#endif
