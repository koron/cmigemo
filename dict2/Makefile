# vim:set ts=8 sts=8 sw=8 tw=0 noet:

HTTP_GET	= curl -O

SKKDICT_BASEURL	= http://openlab.ring.gr.jp/skk/dic
SKKDICT_FILE	= SKK-JISYO.L
DICT_FILE	= migemo-dict
DAT_FILES	= han2zen.dat hira2kata.dat roma2hira.dat zen2han.dat

all: utf-8.d/$(DICT_FILE) euc-jp.d/$(DICT_FILE) cp932.d/$(DICT_FILE)

clean:
	-rm -r utf-8.d euc-jp.d cp932.d
	-rm $(DICT_FILE)

distclean: clean
	-rm $(SKKDICT_FILE)

$(SKKDICT_FILE):
	$(HTTP_GET) $(SKKDICT_BASEURL)/$@.gz
	gzip -d $@.gz
	iconv -f EUC-JISX0213 -t UTF-8 $(SKKDICT_FILE) > $(SKKDICT_FILE).utf-8
	rm $(SKKDICT_FILE) && mv $(SKKDICT_FILE).utf-8 $(SKKDICT_FILE)

$(DICT_FILE): $(SKKDICT_FILE) ../tools/skk2migemo.pl ../tools/optimize-dict.pl
	cat $(SKKDICT_FILE) \
	  | perl ../tools/skk2migemo.pl \
	  | perl ../tools/optimize-dict.pl \
	  > $@

utf-8.d/$(DICT_FILE): $(DICT_FILE) $(DAT_FILES)
	if [ ! -d utf-8.d ] ; then mkdir utf-8.d ; fi
	cp $(DICT_FILE) $(DAT_FILES) utf-8.d

euc-jp.d/$(DICT_FILE): $(DICT_FILE) $(DAT_FILES)
	if [ ! -d euc-jp.d ] ; then mkdir euc-jp.d ; fi
	for i in $(DICT_FILE) $(DAT_FILES) ; do \
	  iconv -f UTF-8 -t EUC-JISX0213 $$i > euc-jp.d/$$i ; \
	done

cp932.d/$(DICT_FILE): $(DICT_FILE) $(DAT_FILES)
	if [ ! -d cp932.d ] ; then mkdir cp932.d ; fi
	for i in $(DICT_FILE) $(DAT_FILES) ; do \
	  iconv -f UTF-8 -t CP932 $$i > cp932.d/$$i ; \
	done
