/* vim:set ts=8 sts=4 sw=4 tw=0: */
/*
 * filename.c - Operate filename.
 *
 * Last change: 15-May-2002.
 * Written by:  Muraoka Taro  <koron@tka.att.ne.jp>
 */

#include <string.h>

/*
 * Cut out base string of filename from filepath.  If base is NULL, then
 * return length that require for store base name.
 */
    int
filename_base(char *base, const char *path)
{
    int i;
    int len = strlen(path) - 1;
    int end;

    for (i = len; i >= 0; --i)
	if (path[i] == '.')
	    break;
    if (i <= 0)
	end = len;
    else
	end = i - 1;
    for (i = end; i >= 0; --i)
	if (path[i] == '\\' || path[i] == '/')
	{
	    ++i;
	    break;
	}
    if (i < 0)
	++i;
    len = end - i + 1;
    if (base)
    {
	strncpy(base, path + i, len);
	base[len] = '\0';
    }
    return len;
}

/*
 * Cut out directroy string from filepath.  If dir is NULL, then return
 * length that require for store directory.
 */
    int
filename_directory(char *dir, const char *path)
{
    int i;

    for (i = strlen(path) - 1; i >= 0; --i)
	if (path[i] == '\\' || path[i] == '/')
	    break;
    if (i <= 0)
    {
	if (dir)
	    dir[0] = '\0';
	return 0;
    }
    if (dir)
    {
	strncpy(dir, path, i + 1);
	dir[i] = '\0';
    }
    return i;
}

/*
 * Cut out extension of filename or filepath. If ext is NULL, then return
 * length that require for store extension.
 */
    int
filename_extension(char *ext, const char *path)
{
    int i;
    int len = strlen(path);

    for (i = len - 1; i >= 0; --i)
	if (path[i] == '.')
	    break;
    if (i < 0 || i == len - 1)
    {
	ext[0] = '\0';
	return 0;
    }
    len -= ++i;
    if (ext)
	strcpy(ext, path + i);
    return len;
}

/*
 * Cut out filename string from filepath.  If name is NULL, then return
 * length that require for store directory.
 */
    int
filename_filename(char *name, const char *path)
{
    int i;
    int len = strlen(path);

    for (i = len - 1; i >= 0; --i)
	if (path[i] == '\\' || path[i] == '/')
	    break;
    ++i;
    len -= i;
    if (name)
    {
	strncpy(name, path + i, len);
	name[len] = '\0';
    }
    return len;
}

/*
 * Generate file full path name.
 */
    int
filename_generate(char *filepath, const char *dir, const char *base,
	const char *ext)
{
    int len = 0;

    if (filepath)
	filepath[0] = '\0';
    if (dir)
    {
	if (filepath)
	{
	    strcat(filepath, dir);
	    strcat(filepath, "/");
	}
	len += strlen(dir) + 1;
    }
    if (base)
    {
	if (filepath)
	    strcat(filepath, base);
	len += strlen(base);
    }
    if (ext)
    {
	if (filepath)
	{
	    strcat(filepath, ".");
	    strcat(filepath, ext);
	}
	len += 1 + strlen(ext);
    }
    return len;
}
