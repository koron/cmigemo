# vim:set ts=8 sts=8 sw=8 tw=0:
#
# 辞書ファイルのメンテナンス
# 
# Last Change: 14-May-2002.
# Written By:  MURAOKA Taro <koron@tka.att.ne.jp>

# 必須プログラム
# 	Perl, cURL, qkc, (cp, rm, mv)

DICT 		= migemo-dict
CURL_FLAGS 	=
SKKDIC_BASEURL 	= http://openlab.ring.gr.jp/skk/dic

RM = rm -f
CP = cp
MV = mv

cp932:		cp932-large
cp932-large:	SKK-JISYO.L $(DICT)
	qkc -q -s -u $(DICT)
cp932-middle:	SKK-JISYO.M $(DICT)
	qkc -q -s -u $(DICT)
cp932-small:	SKK-JISYO.S $(DICT)
	qkc -q -s -u $(DICT)

euc-jp: 	euc-jp-large
euc-jp-large: 	SKK-JISYO.L $(DICT)
euc-jp-middle: 	SKK-JISYO.M $(DICT)
euc-jp-small: 	SKK-JISYO.S $(DICT)

#
# for Cygwin
#
cyg:		cp932-large $(DICT).cp932 $(DICT).euc-jp

#
# for MacOS X
#
osx:		cp932-large $(DICT).cp932 $(DICT).euc-jp

$(DICT): SKK-JISYO.LATEST
	perl ../tools/skk2migemodict.pl < SKK-JISYO.LATEST \
		| perl ../tools/optimize-dict.pl > $@
$(DICT).euc-jp:	$(DICT)
	$(CP) $(DICT) $(DICT).euc-jp
	qkc -q -e -u $(DICT).euc-jp
$(DICT).cp932:	$(DICT)
	$(CP) $(DICT) $(DICT).cp932
	qkc -q -s -u $(DICT).cp932

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

clean:
	$(RM) SKK-JISYO*
	$(RM) $(DICT) $(DICT).euc-jp $(DICT).cp932
