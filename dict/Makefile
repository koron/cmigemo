# vim:set ts=8 sts=8 sw=8 tw=0:
#
# 辞書ファイルのメンテナンス
# 
# Last Change: 14-May-2002.
# Written By:  MURAOKA Taro <koron@tka.att.ne.jp>

# 必須プログラム
# 	Perl, cURL, qkc, cp, rm

DICT 		= migemo-dict
CURL_FLAGS 	=
SKKDIC_BASEURL 	= http://openlab.ring.gr.jp/skk/dic

RM = rm -f
CP = cp

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

$(DICT): SKK-JISYO.LATEST
	perl ../tools/skk2migemodict.pl < SKK-JISYO.LATEST \
		| perl ../tools/optimize-dict.pl > $@
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
	$(RM) $(DICT)
