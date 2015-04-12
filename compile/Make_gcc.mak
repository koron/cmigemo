# vim:set ts=8 sts=8 sw=8 tw=0:
#
# GNU/gcc(Linux��)�pMakefile
#
# Last Change:	19-Oct-2003.
# Base Idea:	AIDA Shinra
# Maintainer:	MURAOKA Taro <koron@tka.att.ne.jp>

##############################################################################
# ���ɉ����Ă��̕ϐ���ύX����
#
libmigemo_LIB	= libmigemo.so.1.1.0
libmigemo_DSO	= libmigemo.so.1
libmigemo	= libmigemo.so
EXEEXT		=
CFLAGS_MIGEMO	= -fPIC
LDFLAGS_MIGEMO	= -Wl,-rpath,.,-rpath,/usr/lib

include config.mk
include compile/unix.mak
include src/depend.mak
include compile/clean_unix.mak
include compile/clean.mak

##############################################################################
# ���ɉ����ă��C�u�����\�z�@��ύX����
#
$(libmigemo_LIB): $(libmigemo_DSO)
$(libmigemo_DSO): $(libmigemo_OBJ)
	$(CC) -shared -o $(libmigemo_LIB) -Wl,-soname,$@ $(libmigemo_OBJ)
	$(RM) $@ $(libmigemo)
	ln -s $(libmigemo_LIB) $@
	ln -s $(libmigemo_LIB) $(libmigemo)

install-lib: $(libmigemo_DSO)
	$(INSTALL_PROGRAM) $(libmigemo_LIB) $(libdir)
	$(RM) $(libdir)/$(libmigemo_DSO) $(libdir)/$(libmigemo)
	ln -s $(libmigemo_LIB) $(libdir)/$(libmigemo_DSO)
	ln -s $(libmigemo_LIB) $(libdir)/$(libmigemo)

uninstall-lib:
	$(RM) $(libdir)/$(libmigemo_DSO)
	$(RM) $(libdir)/$(libmigemo_LIB)
	$(RM) $(libdir)/$(libmigemo)

dictionary:
	cd dict && $(MAKE) gcc
