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
libmigemo_LIB = libmigemo.1.dylib
libmigemo_DSO = libmigemo.dylib
EXEEXT =

include compile/unix.mak

##############################################################################
# 環境に応じてライブラリ構築法を変更する
#
$(libmigemo_LIB): $(libmigemo_DSO)
$(libmigemo_DSO): $(libmigemo_OBJ)
	$(CC) -dynamiclib -install_name $@ -o $(libmigemo_LIB) $(libmigemo_OBJ)
	ln -s $(libmigemo_LIB) $@

install-lib: $(libmigemo_DSO)
	$(INSTALL_DATA) $(libmigemo_LIB) $(libdir)
	ln -s $(libdir)/$(libmigemo_LIB) $(libdir)/$(libmigemo_DSO)

uninstall-lib:
	$(RM) $(libdir)/$(libmigemo_DSO)
	$(RM) $(libdir)/$(libmigemo_LIB)
