# vi:set ts=8 sts=8 sw=8 tw=0:
#
# C/Migemo Makefile
#
# Last Change: 14-May-2002.
# Written By:  MURAOKA Taro <koron@tka.att.ne.jp>

RM = rm -f 

tags: *.c *.h
	ctags *.c *.h

#
# for Microsoft Visual C
#
msvc: msvc-rel
msvc-all: msvc-rel msvc-dict
msvc-rel:
	$(MAKE) /nologo /f compile\migemo.mak CFG="migemo - Win32 Release"
msvc-dbg:
	$(MAKE) /nologo /f compile\migemo.mak CFG="migemo - Win32 Debug"
msvc-dict:
	cd dict
	$(MAKE) /nologo cp932

#
# for Cygwin
#
cyg: cyg-rel
cyg-all: cyg-rel cyg-dict
cyg-rel:
	$(MAKE) -f compile/Make_cyg.mak
cyg-dict:
	cd dict && $(MAKE) cyg
cyg-install: cyg-all
	$(MAKE) -f compile/Make_cyg.mak install
cyg-uninstall:
	$(MAKE) -f compile/Make_cyg.mak uninstall
cyg-clean:
	$(MAKE) -f compile/Make_cyg.mak clean

#
# for MacOS X
#
osx: osx-rel
osx-all: osx-rel osx-dict
osx-rel:
	$(MAKE) -f compile/Make_osx.mak
osx-dict:
	cd dict && $(MAKE) osx
osx-install: osx-all
	$(MAKE) -f compile/Make_osx.mak install
osx-uninstall:
	$(MAKE) -f compile/Make_osx.mak uninstall
osx-clean:
	$(MAKE) -f compile/Make_osx.mak clean

#
# Cleaning
#
clean:
	$(RM) *.o *.a
	$(RM) *.dylib
	$(RM) *.dll *.exe
dict-clean:
	cd dict ; $(MAKE) clean
distclean: clean dict-clean
	$(RM) -rf Release Debug migemo.opt migemo.ncb
	$(RM) tags
