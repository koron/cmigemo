// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// filename.c - Operate filename.
//
// Written by:  Muraoka Taro  <koron.kaoriya@gmail.com>

#include <limits.h>
#include <string.h>

static size_t
my_strlen(const char *s)
{
    size_t len = strlen(s);
    return len <= INT_MAX ? len : INT_MAX;
}

// Cut out directroy string from filepath.
//
// Copies the part of `path` preceding the final `\` or `/` into `dir`. `cap`
// is the size of the buffer allocated for `dir`, including the terminating
// null character. If the size is insufficient, no copy is performed. The
// return value is the size required for the copy, including the null
// character.
//
// Returns an empty string for the root path.
size_t
filename_directory(char *dir, size_t cap, const char *path)
{
    const char *p = path + my_strlen(path);
    for (; p > path; p--)
        if (*p == '\\' || *p == '/')
            break;
    // Returns an empty string if the path is from the root or does not contain
    // a directory separator.
    if (p == path)
    {
        if (cap > 0 && dir)
            dir[0] = '\0';
        return 1;
    }
    // Returns the string up to, but not including, the last directory
    // separator.
    size_t len = p - path;
    if (cap >= len + 1 && dir)
    {
        memcpy(dir, path, len);
        dir[len] = '\0';
    }
    return len + 1;
}

size_t
filename_join(char *out, size_t cap, const char *dir, const char *file)
{
    size_t len_d = my_strlen(dir);
    // Strip trailing directory separators.
    while (len_d > 0 && (dir[len_d - 1] == '/' || dir[len_d - 1] == '\\'))
        len_d--;

    size_t len_f = my_strlen(file);
    size_t total = len_d + 1 + len_f + 1;
    if (cap >= total && out)
    {
        memcpy(out, dir, len_d);
        out[len_d] = '/';
        memcpy(out + len_d + 1, file, len_f);
        out[len_d + 1 + len_f] = '\0';
    }
    return total;
}
