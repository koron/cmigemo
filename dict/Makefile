# vim:set ts=8 sts=8 sw=8 tw=0:
#
# 辞書ファイルのメンテナンス
# 
# Last Change: 15-May-2002.
# Written By:  MURAOKA Taro <koron@tka.att.ne.jp>

# 必須プログラム
# 	Perl, cURL, qkc, (cp, rm, mkdir)

DICT 		= migemo-dict
SKKDIC_BASEURL 	= http://openlab.ring.gr.jp/skk/dic

include ../config.mk

##############################################################################
# Dictionary
#
$(DICT): SKK-JISYO.LATEST
	$(PERL) ../tools/skk2migemodict.pl < SKK-JISYO.LATEST \
		| perl ../tools/optimize-dict.pl | $(FILTER_CP932) > $@
SKK-JISYO.S:
	$(HTTP) $(SKKDIC_BASEURL)/$@.bz2
	$(BUNZIP2) $@.bz2
	$(CP) $@ SKK-JISYO.LATEST
SKK-JISYO.M:
	$(HTTP) $(SKKDIC_BASEURL)/$@.bz2
	$(BUNZIP2) $@.bz2
	$(CP) $@ SKK-JISYO.LATEST
SKK-JISYO.L:
	$(HTTP) $(SKKDIC_BASEURL)/$@.bz2
	$(BUNZIP2) $@.bz2
	$(CP) $@ SKK-JISYO.LATEST

##############################################################################
# Dictionary in cp932
#
cp932:		cp932-large
cp932-large:	SKK-JISYO.L $(DICT)
cp932-middle:	SKK-JISYO.M $(DICT)
cp932-small:	SKK-JISYO.S $(DICT)

##############################################################################
# Dictionary in euc-jp
#
EUCJP_DIR = euc-jp.d
euc-jp: 	cp932-large euc-jp-files
$(EUCJP_DIR):
	$(MKDIR) $(EUCJP_DIR)
euc-jp-files: $(EUCJP_DIR) $(EUCJP_DIR)/migemo-dict $(EUCJP_DIR)/han2zen.dat \
		$(EUCJP_DIR)/hira2kata.dat $(EUCJP_DIR)/roma2hira.dat
$(EUCJP_DIR)/migemo-dict: migemo-dict
	$(FILTER_EUCJP) < $< > $@
$(EUCJP_DIR)/han2zen.dat: han2zen.dat
	$(FILTER_EUCJP) < $< > $@
$(EUCJP_DIR)/hira2kata.dat: hira2kata.dat
	$(FILTER_EUCJP) < $< > $@
$(EUCJP_DIR)/roma2hira.dat: roma2hira.dat
	$(FILTER_EUCJP) < $< > $@

##############################################################################
# for Cygwin
#
cyg:		euc-jp

##############################################################################
# for MacOS X
#
osx:		euc-jp

##############################################################################
# for Linux
#
linux:		euc-jp

##############################################################################
# Cleaning
#
clean:
	$(RMDIR) euc-jp.d
distclean: clean
	$(RM) $(DICT)
	$(RM) SKK-JISYO*
