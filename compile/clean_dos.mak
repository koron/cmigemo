# vim:set ts=8 sts=8 sw=8 tw=0:
#
# Clean up アーキテクチャ依存 (DOS/Windows)
#
# Last Change:	16-May-2002.
# Written By:	MURAOKA Taro <koron@tka.att.ne.jp>

RM = del /F /Q
RMDIR = rmdir /S /Q

clean-arch:
	-$(RM) dict\migemo-dict

distclean-arch: clean
	-$(RM) dict\SKK-JISYO*
	-$(RM) dict\base-dict
	-$(RMDIR) dict\euc-jp.d
