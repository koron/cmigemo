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

dict-clean:
	cd dict ; $(MAKE) clean

distclean: dict-clean
	$(RM) -fr Release Debug migemo.opt migemo.ncb
	$(RM) *.o *.a
	$(RM) *.dll *.exe
	$(RM) tags
