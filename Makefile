# vi:set ts=8 sts=8 sw=8 tw=0:
#
# Makefile
#
# Last Change: 14-May-2002.
# Written By:  MURAOKA Taro <koron@tka.att.ne.jp>

tags: *.c *.h
	ctags *.c *.h

msvc: msvc-rel
msvc-rel:
	$(MAKE) /nologo /f compile\migemo.mak CFG="migemo - Win32 Release"
msvc-dbg:
	$(MAKE) /nologo /f compile\migemo.mak CFG="migemo - Win32 Debug"
