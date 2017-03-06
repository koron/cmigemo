# vim:set ts=8 sts=8 sw=8 tw=0:
#
# デフォルトコンフィギュレーションファイル
#
# Last Change:	07-Mar-2014.
# Base Idea:	AIDA Shinra
# Maintainer:	MURAOKA Taro <koron.kaoriya@gmail.com>

srcdir = ./src/
objdir = ./build/object/
outdir = ./build/

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
#HTTP		= wget
PERL		= perl
BUNZIP2		= bzip2 -d
GUNZIP		= gzip -d
ICONV_EUCJP_TO_CP932 = iconv -f euc-jp-ms -t cp932
ICONV_CP932_TO_EUCJP = iconv -f cp932 -t euc-jp-ms
ICONV_CP932_TO_UTF8  = iconv -f cp932 -t utf-8
INSTALL		= /usr/bin/install -c
#INSTALL	= /usr/ucb/install -c
INSTALL_PROGRAM	= $(INSTALL) -m 755
INSTALL_DATA	= $(INSTALL) -m 644

##############################################################################
# 定数
#
O		= o
EXE		=
CONFIG_DEFAULT	= compile/config_default.mk
CONFIG_IN	= compile/config.mk.in
