# vim:set ts=8 sts=8 sw=8 tw=0:
#
# Clean up - dependent on DOS/Windows
#
# Last Change:	17-Nov-2021.
# Written By:	MURAOKA Taro <koron.kaoriya@gmail.com>

# 次行の\のあとには半角スペースが必要→"\ "
DICT_DIR = dict\ 

# BC5では空のチャンクはエラーになるので"\t@"が必要
clean-arch:
	@

distclean-arch:
	@
