# vim:set ts=8 sts=8 sw=8 tw=0:
#
# UNIXŒn‹¤’ÊMakefile
#
# Last Change:	19-Oct-2003.
# Base Idea:	AIDA Shinra
# Maintainer:	MURAOKA Taro <koron@tka.att.ne.jp>

libmigemo_SRC = $(SRC)
libmigemo_OBJ = $(OBJ)

DEFINES	=
CFLAGS	= -O2 -Wall $(DEFINES) $(CFLAGS_MIGEMO)
LDFLAGS = $(LDFLAGS_MIGEMO)
LIBS	= 

default: dirs $(outdir)cmigemo$(EXEEXT)

dirs:
	@for i in $(objdir) $(outdir); do \
		if test ! -d $$i; then \
			$(MKDIR) $$i; \
		fi \
	done

$(outdir)cmigemo$(EXEEXT): $(objdir)main.$(O) $(libmigemo_LIB)
	$(CC) -o $@ $(objdir)main.$(O) -L. -L$(outdir) -lmigemo $(LDFLAGS)

$(objdir)main.o: $(srcdir)main.c
	$(CC) $(CFLAGS) -o $@ -c $<

$(objdir)%.o: $(srcdir)%.c
	$(CC) $(CFLAGS) -o $@ -c $<

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
	if [ -d dict/euc-jp.d ]; then \
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
