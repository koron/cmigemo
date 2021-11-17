# vim:set ts=8 sts=8 sw=8 tw=0:
#
# Default configuration file.
#
# Last Change:	17-Nov-2021.
# Base Idea:	AIDA Shinra
# Maintainer:	MURAOKA Taro <koron.kaoriya@gmail.com>

srcdir = ./src/
objdir = ./build/object/
outdir = ./build/

##############################################################################
# for install directories
#
prefix		= /usr/local
bindir		= $(prefix)/bin
libdir		= $(prefix)/lib
incdir		= $(prefix)/include
# WARNING: $(dictdir) and $(docdir) will be removed with its contents when
# uninstall.
dictdir		= $(prefix)/share/migemo
docdir		= $(prefix)/doc/migemo

##############################################################################
# for commands
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
# Constants
#
O		= o
EXE		=
CONFIG_DEFAULT	= compile/config_default.mk
CONFIG_IN	= compile/config.mk.in
