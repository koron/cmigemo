# vim:set ts=8 sts=8 sw=8 tw=0:
#
# Borland C 5—p Makefile
#
# Last Change:	16-May-2002.
# Written By:	MURAOKA Taro <koron@tka.att.ne.jp>

default: release

!include config.mk
!include compile/clean_dos.mak
!include compile/clean.mak
!include dict/dict.mak

release:

dictionary: cd-dict bc5
	cd ..

cd-dict:
	cd dict
