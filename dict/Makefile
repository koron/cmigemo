# vim:set ts=8 sts=8 sw=8 tw=0:
#
# 辞書ファイルのメンテナンス
# 
# Last Change: 14-May-2002.
# Written By:  MURAOKA Taro <koron@tka.att.ne.jp>

# 必須プログラム
# 	Perl, cURL, qkc, (cp, rm, mv, mkdir)

DICT 		= migemo-dict
CURL_FLAGS 	=
SKKDIC_BASEURL 	= http://openlab.ring.gr.jp/skk/dic

RM = rm -f
CP = cp
MV = mv
MKDIR = mkdir -p

cp932:		cp932-large
cp932-large:	SKK-JISYO.L $(DICT)
cp932-middle:	SKK-JISYO.M $(DICT)
cp932-small:	SKK-JISYO.S $(DICT)

EUCJP_DIR = euc-jp.d
euc-jp: 	cp932-large euc-jp-files
$(EUCJP_DIR):
	$(MKDIR) $(EUCJP_DIR)
euc-jp-files: $(EUCJP_DIR) $(EUCJP_DIR)/migemo-dict $(EUCJP_DIR)/han2zen.dat \
		$(EUCJP_DIR)/hira2kata.dat $(EUCJP_DIR)/roma2hira.dat
$(EUCJP_DIR)/migemo-dict: migemo-dict
	qkc -q -e -u < $< > $@
$(EUCJP_DIR)/han2zen.dat: han2zen.dat
	qkc -q -e -u < $< > $@
$(EUCJP_DIR)/hira2kata.dat: hira2kata.dat
	qkc -q -e -u < $< > $@
$(EUCJP_DIR)/roma2hira.dat: roma2hira.dat
	qkc -q -e -u < $< > $@

#
# for Cygwin
#
cyg:		euc-jp

#
# for MacOS X
#
osx:		euc-jp

$(DICT): SKK-JISYO.LATEST
	perl ../tools/skk2migemodict.pl < SKK-JISYO.LATEST \
		| perl ../tools/optimize-dict.pl > $@
	qkc -q -s -u $@
SKK-JISYO.S:
	curl $(CURL_FLAGS) -O $(SKKDIC_BASEURL)/$@.bz2
	bzip2 -d $@.bz2
	$(CP) $@ SKK-JISYO.LATEST
SKK-JISYO.M:
	curl $(CURL_FLAGS) -O $(SKKDIC_BASEURL)/$@.bz2
	bzip2 -d $@.bz2
	$(CP) $@ SKK-JISYO.LATEST
SKK-JISYO.L:
	curl $(CURL_FLAGS) -O $(SKKDIC_BASEURL)/$@.bz2
	bzip2 -d $@.bz2
	$(CP) $@ SKK-JISYO.LATEST

#
# Cleaning
#
clean:
	$(RM) -r euc-jp.d
distclean: clean
	$(RM) $(DICT)
	$(RM) SKK-JISYO*
