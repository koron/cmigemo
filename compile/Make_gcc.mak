# vim:set ts=8 sts=8 sw=8 tw=0:
#
# Cygwin用Makefile
#
# Last Change:	15-May-2002.
# Base Idea:	AIDA Shinra
# Written By:	MURAOKA Taro <koron@tka.att.ne.jp>

##############################################################################
# 環境に応じてこの3変数を変更する
#
libmigemo_LIB = libmigemo.so.1.0
libmigemo_DSO = libmigemo.so.1
EXEEXT =
CFLAGS_MIGEMO = -fPIC
LDFLAGS_MIGEMO = -Wl,-rpath,.,-rpath,/usr/local/lib,-rpath,/usr/lib

include compile/unix.mak

##############################################################################
# 環境に応じてライブラリ構築法を変更する
#
$(libmigemo_LIB): $(libmigemo_DSO)
$(libmigemo_DSO): $(libmigemo_OBJ)
	$(CC) -shared -o $(libmigemo_LIB) -Wl,-soname,$@ $(libmigemo_OBJ)
	$(RM) $@ libmigemo.so
	ln -s $(libmigemo_LIB) $@
	ln -s $(libmigemo_LIB) libmigemo.so

install-lib: $(libmigemo_DSO) $(libmigemo_LIB)
	$(INSTALL_PROGRAM) $(libmigemo_LIB) $(libdir)
	$(RM) $(libdir)/$(libmigemo_DSO) $(libdir)/libmigemo.so
	ln -s $(libmigemo_LIB) $(libdir)/$(libmigemo_DSO)
	ln -s $(libmigemo_LIB) $(libdir)/libmigemo.so

uninstall-lib:
	$(RM) $(libdir)/$(libmigemo_DSO)
	$(RM) $(libdir)/$(libmigemo_LIB)
	$(RM) $(libdir)/libmigemo.so
