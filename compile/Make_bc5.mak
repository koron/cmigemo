# vim:set ts=8 sts=8 sw=8 tw=0:
#
# Borland C 5óp Makefile
#
# Last Change:	18-Oct-2003.
# Written By:	MURAOKA Taro <koron@tka.att.ne.jp>

# éQçléëóø:
#	http://www2.justnet.ne.jp/~tyche/bcbbugs/bcc32-option.html
#	http://www2.justnet.ne.jp/~tyche/bcbbugs/ilink32-option.html

default: rel

!include config.mk
!include compile\dos.mak
!include compile\clean_dos.mak
!include compile\clean.mak
!include dict\dict.mak

libmigemo_LIB = migemo.lib
libmigemo_DSO = migemo.dll
libmigemo_SRC = \
		filename.c migemo.c mnode.c romaji.c \
		rxgen.c wordbuf.c wordlist.c
libmigemo_OBJ = $(libmigemo_SRC:.c=.obj)

DEFINES	= -DWIN32 -D_CONSOLE
CFLAGS	= -O2 -G -pr -w- -VM -WM $(DEFINES) $(CFLAGS_MIGEMO)
LDFLAGS = -x -Gn -w- $(LDFLAGS_MIGEMO)
LIBS	= import32.lib cw32mt.lib

LD = ilink32

rel: cmigemo.exe

cmigemo.exe: main.obj $(libmigemo_LIB)
	$(LD) $(LDFLAGS) c0x32.obj main.obj, $@, , $(libmigemo_LIB) $(LIBS), ,

$(libmigemo_LIB): $(libmigemo_DSO)
$(libmigemo_DSO): $(libmigemo_OBJ) migemo.def
	$(LD) $(LDFLAGS) -Tpd -Gi $(libmigemo_OBJ) c0d32.obj, $@, , $(LIBS), migemo.def,

dictionary: cd-dict bc5
	cd ..

cd-dict:
	cd dict
