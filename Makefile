# vi:set ts=8 sts=8 sw=8 tw=0:
#
# Makefile -
#
# Last Change: 07-Aug-2001.
# Written By:  Muraoka Taro  <koron@tka.att.ne.jp>

RELEASE=yes
SRC = migemo.c wordbuf.c wordlist.c mnode.c rxgen.c romaji.c filename.c main.c
OBJ = $(SRC:.c=.obj)

CFLAGS = -W3
DEFINES = -DWIN32 -D_CONSOLE
INCDIRS = -I.
#LDFLAGS = -LIBPATH:./lib
LDFLAGS =
LIBS =

project: migemo.exe

rxgen.exe: rxgen.obj wordbuf.obj
migemo.exe: $(OBJ)

migemo.obj:: wordbuf.h

###############################################################################
CC = cl -nologo
AS =
LD = link -nologo
AR = lib -nologo
RC = rc
RM = rm -f
MD = mkdir -p
CP = cp -p
CTAGS = ctags
TAR = tar

ifdef MULTI_THREAD
  CFLAGS_LINKING = -MT
else
  CFLAGS_LINKING = -ML
endif

ifdef RELEASE
  DEFINES += -DNDEBUG
  CFLAGS += -G6 -O2 $(CFLAGS_LINKING)
else
  DEFINES += -D_DEBUG
  CFLAGS += -Zi $(CFLAGS_LINKING)d
endif

%.obj: %.c
	$(CC) $(CFLAGS) $(DEFINES) $(INCDIRS) -c $< -Fo$@
%.lib:
	$(AR) $^ /OUT:$@
%.exe:
	$(CC) $(CFLAGS) $^ $(LIBS) -Fe$@ -link $(LDFLAGS)
tags: *.c *.h
	$(CTAGS) *.c *.h

.PHONY: clean distclean allclean
clean:
	$(RM) *.obj Release/*.obj Debug/*.obj Profile/*.obj
distclean: clean
	$(RM) tags
	$(RM) *.exe *.lib *.dll *.exp
	$(RM) *.pdb *.ilk *.opt
	$(RM) *.plg *.opt *.ncb
	$(RM) -r Release Debug Profile
allclean: distclean
