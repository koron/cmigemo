# vim:set ts=8 sts=8 sw=8 tw=0:
#
# UNIXŒn‹¤’ÊMakefile
#
# Last Change:	15-May-2002.
# Base Idea:	AIDA Shinra
# Written By:	MURAOKA Taro <koron@tka.att.ne.jp>

!include config.mk

libmigemo_SRC = \
		filename.c migemo.c mnode.c romaji.c \
		rxgen.c wordbuf.c wordlist.c
libmigemo_OBJ = $(libmigemo_SRC:.c=.o)

DEFINES	=
CFLAGS	= -O2 -Wall $(DEFINES) $(CFLAGS_MIGEMO)
LDFLAGS = $(LDFLAGS_MIGEMO)
LIBS	= 

default: cmigemo$(EXEEXT)

cmigemo$(EXEEXT): main.o $(libmigemo_LIB)
	$(CC) -o $@ main.o -L. -lmigemo $(LDFLAGS)

main.o: main.c
	$(CC) -c $< -o $@ -D_SPLITED_MIGEMO $(CFLAGS)

##############################################################################
# Install
#
install-mkdir:
	$(MKDIR) $(bindir)
	$(MKDIR) $(libdir)
	$(MKDIR) $(incdir)
	$(MKDIR) $(docdir)
	$(MKDIR) $(dictdir)
	$(MKDIR) $(dictdir)/cp932
	$(MKDIR) $(dictdir)/euc-jp

install-dict:
	$(INSTALL_DATA) dict/migemo-dict $(dictdir)/cp932
	$(INSTALL_DATA) dict/han2zen.dat $(dictdir)/cp932
	$(INSTALL_DATA) dict/hira2kata.dat $(dictdir)/cp932
	$(INSTALL_DATA) dict/roma2hira.dat $(dictdir)/cp932
	if [ -e dict/euc-jp.d ]; then \
	  $(INSTALL_DATA) dict/euc-jp.d/migemo-dict $(dictdir)/euc-jp; \
	  $(INSTALL_DATA) dict/euc-jp.d/han2zen.dat $(dictdir)/euc-jp; \
	  $(INSTALL_DATA) dict/euc-jp.d/hira2kata.dat $(dictdir)/euc-jp; \
	  $(INSTALL_DATA) dict/euc-jp.d/roma2hira.dat $(dictdir)/euc-jp; \
	fi

install: cmigemo$(EXEEXT) $(libmigemo_DSO) install-mkdir install-dict install-lib
	$(INSTALL_DATA) migemo.h $(incdir)
	$(INSTALL_DATA) doc/README_j.txt $(docdir)
	$(INSTALL_PROGRAM) cmigemo$(EXEEXT) $(bindir)

##############################################################################
# Uninstall
#
uninstall: uninstall-lib
	$(RM) $(dictdir)/migemo-dict*
	$(RM) $(incdir)/migemo.h
	$(RM) $(docdir)/README_j.txt
	$(RM) $(bindir)/cmigemo$(EXEEXT)
	$(RMDIR) $(dictdir)
	$(RMDIR) $(docdir)

##############################################################################
# Cleaning
#
clean:
	$(RM) *.o
	$(RM) cmigemo$(EXEEXT)
	$(RM) $(CLEAN_TARGET)
