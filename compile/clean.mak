# vim:set ts=8 sts=8 sw=8 tw=0:
#
# Clean up アーキテクチャ非依存
#
# Last Change:	16-May-2002.
# Written By:	MURAOKA Taro <koron@tka.att.ne.jp>

clean: clean-arch
	-$(RM) *.o
	-$(RM) *.obj
	-$(RM) $(DICT_DIR)migemo-dict
	-$(RMDIR) Release
	-$(RMDIR) Debug

distclean: clean distclean-arch
	-$(RM) *.a
	-$(RM) *.dll
	-$(RM) *.dylib
	-$(RM) *.exe
	-$(RM) *.lib
	-$(RM) *.tds
	-$(RM) cmigemo
	-$(RM) libmigemo.*.dylib
	-$(RM) libmigemo.so*
	-$(RM) migemo.opt
	-$(RM) migemo.ncb
	-$(RM) tags
	-$(RM) $(DICT_DIR)SKK-JISYO*
	-$(RM) $(DICT_DIR)base-dict
	-$(RMDIR) $(DICT_DIR)euc-jp.d
