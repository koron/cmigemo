# vim:set ts=8 sts=8 sw=8 tw=0:
#
# デフォルトコンフィギュレーションファイル
#
# Last Change:	15-May-2002.
# Base Idea:	AIDA Shinra
# Written By:	MURAOKA Taro <koron@tka.att.ne.jp>

##############################################################################
# インストールディレクトリの設定
#
prefix		= /usr/local
bindir		= $(prefix)/bin
libdir		= $(prefix)/lib
incdir		= $(prefix)/include
# 警告: $(dictdir)と$(docdir)はアンインストール実行時にディレクトリごと消去
# されます。
dictdir		= $(prefix)/share/migemo
docdir		= $(prefix)/doc/migemo

##############################################################################
# コマンド設定
#
RM		= rm -f
CP		= cp
MKDIR		= mkdir -p
RMDIR		= rm -rf
CTAGS		= ctags
HTTP		= curl -O
PERL		= perl
BUNZIP2		= bzip2 -d
FILTER_CP932	= qkc -q -u -s
FILTER_EUCJP	= qkc -q -u -e
INSTALL		= /usr/bin/install -c
INSTALL_PROGRAM	= $(INSTALL) -m 755
INSTALL_DATA	= $(INSTALL) -m 644
