# vim:set ts=8 sts=8 sw=8 tw=0:
#
# Clean up アーキテクチャ依存 (DOS/Windows)
#
# Last Change:	16-May-2002.
# Written By:	MURAOKA Taro <koron@tka.att.ne.jp>

RM = del /F /Q
RMDIR = rd /S /Q
# 次行の\のあとには半角スペースが必要→"\ "
DICT_DIR = dict\ 

clean-arch:

distclean-arch:
