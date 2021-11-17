# vim:set ts=8 sts=8 sw=8 tw=0:
#
# Dependent on DOS/Windows
#
# Last Change:	17-Nov-2021.
# Written By:	MURAOKA Taro <koron.kaoriya@gmail.com>
# Maintainer:	MURAOKA Taro <koron.kaoriya@gmail.com>

srcdir = .\src\				#
objdir = .\build\object\		#
outdir = .\build\			#
# Borland's make command accepts last directory separator (\) in value if
# comment at EOL.

CP = copy
MKDIR = mkdir
RM = del /F /Q
RMDIR = rd /S /Q

O		= obj
EXE		= .exe
CONFIG_DEFAULT	= compile\config_default.mk
CONFIG_IN	= compile\config.mk.in
