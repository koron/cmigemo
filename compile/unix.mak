# vim:set ts=8 sts=8 sw=8 tw=0:
#
# UNIXŒn‹¤’ÊMakefile
#
# Last Change:	14-May-2002.
# Base Idea:	AIDA Shinra
# Written By:	MURAOKA Taro <koron@tka.att.ne.jp>

prefix	= /usr/local
bindir	= $(prefix)/bin
libdir	= $(prefix)/lib
incdir	= $(prefix)/include
dictdir	= $(prefix)/share/migemo
docdir	= $(prefix)/doc/migemo
# WARNING:
#	Directory $(dictdir) and $(docdir) will be deleted when uninstall.

RM = rm -f
MKDIR = mkdir -p
INSTALL = /bin/install -c
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA = $(INSTALL) -m 644

libmigemo_SRC = \
		filename.c migemo.c mnode.c romaji.c \
		rxgen.c wordbuf.c wordlist.c
libmigemo_OBJ = $(libmigemo_SRC:.c=.o)

DEFINES	=
CFLAGS	= -O2 -Wall $(DEFINES)
LDFLAGS = 
LIBS	= 

default: cmigemo$(EXEEXT)

cmigemo$(EXEEXT): main.o $(libmigemo_LIB)
	$(CC) -o $@ main.o $(libmigemo_LIB)

main.o: main.c
	$(CC) -c $< -o $@ -D_SPLITED_MIGEMO $(CFLAGS)

install: cmigemo$(EXEEXT) $(libmigemo_DSO)
	$(MKDIR) $(bindir)
	$(MKDIR) $(libdir)
	$(MKDIR) $(incdir)
	$(MKDIR) $(docdir)
	$(MKDIR) $(dictdir)
	$(INSTALL_PROGRAM) cmigemo$(EXEEXT) $(bindir)
	$(INSTALL_PROGRAM) $(libmigemo_DSO) $(bindir)
	if [ -e dict/migemo-dict ]; then \
		$(INSTALL_DATA) dict/migemo-dict $(dictdir); \
	fi
	if [ -e dict/migemo-dict.cp932 ]; then \
		$(INSTALL_DATA) dict/migemo-dict.cp932 $(dictdir); \
	fi
	if [ -e dict/migemo-dict.euc-jp ]; then \
		$(INSTALL_DATA) dict/migemo-dict.euc-jp $(dictdir); \
	fi
	$(INSTALL_DATA) doc/README_j.txt $(docdir)
	$(INSTALL_DATA) $(libmigemo_LIB) $(libdir)
	$(INSTALL_DATA) migemo.h $(incdir)

uninstall:
	$(RM) $(bindir)/cmigemo$(EXEEXT)
	$(RM) $(bindir)/$(libmigemo_DSO)
	$(RM) $(dictdir)/migemo-dict*
	$(RM) $(docdir)/README_j.txt
	$(RM) $(libdir)/$(libmigemo_LIB)
	$(RM) -r $(incdir)/migemo.h
	$(RM) -r $(dictdir) $(docdir)

clean:
	$(RM) *.o
	$(RM) $(libmigemo_LIB) $(libmigemo_DSO)
	$(RM) cmigemo$(EXEEXT)
