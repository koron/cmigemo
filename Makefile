# vi:set ts=8 sts=8 sw=8 tw=0:
#
# Makefile -
#
# Last Change: 14-May-2002.
# Written By:  Muraoka Taro  <koron@tka.att.ne.jp>

tags: *.c *.h
	ctags *.c *.h

msvc:
	$(MAKE) /f Make_mvc.mak
