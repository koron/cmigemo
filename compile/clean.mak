# vim:set ts=8 sts=8 sw=8 tw=0:
#
# Clean up アーキテクチャ非依存
#
# Last Change:	17-May-2002.
# Written By:	MURAOKA Taro <koron@tka.att.ne.jp>

clean-migemo:
	-$(RM) $(DICT_DIR)migemo-dict

distclean-migemo:
	-$(RM) cmigemo
	-$(RM) cmigemo.exe
	-$(RM) libmigemo.*.dylib
	-$(RM) libmigemo.so*
	-$(RM) migemo.opt
	-$(RM) migemo.ncb
	-$(RM) $(DICT_DIR)SKK-JISYO*
	-$(RM) $(DICT_DIR)base-dict
	-$(RMDIR) $(DICT_DIR)euc-jp.d

clean: clean-arch clean-migemo
	-$(RM) *.a
	-$(RM) *.o
	-$(RM) *.obj
	-$(RM) *.lib
	-$(RM) *.tds
	-$(RMDIR) Release
	-$(RMDIR) Debug

distclean: clean distclean-arch distclean-migemo
	-$(RM) *.dll
	-$(RM) *.dylib
	-$(RM) tags
