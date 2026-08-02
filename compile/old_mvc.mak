# vim:set ts=8 sts=8 sw=8 tw=0:
#
# Visual C++用 Makefile
#
# Written By:	MURAOKA Taro <koron.kaoriya@gmail.com>

default: rel

!include config.mk
!include compile/clean_dos.mak
!include compile/clean.mak
!include dict/dict.mak

rel:
	$(MAKE) /nologo /f compile\migemo.mak CFG="migemo - Win32 Release"

dbg:
	$(MAKE) /nologo /f compile\migemo.mak CFG="migemo - Win32 Debug"

dictionary: cd-dict msvc
	cd ..

cd-dict:
	cd dict
