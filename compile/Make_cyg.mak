# vim:set ts=8 sts=8 sw=8 tw=0:
#
# Cygwin用Makefile
#
# Last Change:	14-May-2002.
# Base Idea:	AIDA Shinra
# Written By:	MURAOKA Taro <koron@tka.att.ne.jp>

##############################################################################
# 環境に応じてこの3変数を変更する
#
libmigemo_LIB = libmigemo.dll.a
libmigemo_DSO = cygmigemo1.dll
EXEEXT = .exe

include compile/unix.mak

##############################################################################
# 環境に応じてライブラリ構築法を変更する
#
$(libmigemo_LIB): $(libmigemo_DSO)
$(libmigemo_DSO): $(libmigemo_OBJ) migemo.def
	dllwrap --dllname $(libmigemo_DSO) --implib $(libmigemo_LIB) --def migemo.def $(libmigemo_OBJ)
