# vim:set ts=8 sts=8 sw=8 tw=0:
#
# アーキテクチャ依存 (DOS/Windows)
#
# Last Change:	19-Oct-2003.
# Written By:	MURAOKA Taro <koron@tka.att.ne.jp>
# Maintainer:	MURAOKA Taro <koron@tka.att.ne.jp>

srcdir = .\src\				#
objdir = .\build\object\		#
outdir = .\build\			#
# Borlandのmakeでは後ろにコメントを付けることで行末に\を含めることができる

MKDIR = mkdir
RM = del /F /Q
RMDIR = rd /S /Q

O = obj
EXE = .exe
