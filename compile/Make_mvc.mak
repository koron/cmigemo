# vim:set ts=8 sts=8 sw=8 tw=0:
#
# Visual C++—p Makefile
#
# Last Change:	18-Oct-2003.
# Written By:	MURAOKA Taro <koron@tka.att.ne.jp>

default: rel

!include config.mk
!include compile\dos.mak
!include depend.mak
!include compile\clean_dos.mak
!include compile\clean.mak
!include dict\dict.mak

libmigemo_LIB = migemo.lib
libmigemo_DSO = migemo.dll
libmigemo_SRC = $(SRC)
libmigemo_OBJ = $(OBJ)

!ifndef DEBUG
DEFINES = -DNDEBUG
CFLAGS	= -G6 -W3 -O2 -MT
!else
DEFINES = -D_DEBUG
CFLAGS	= -Zi -W3 -Od -MTd
!endif
DEFINES	= -DWIN32 -D_WINDOWS -D_CONSOLE -D_MBCS $(DEFINES)
CFLAGS	= -nologo $(CFLAGS) $(DEFINES) $(CFLAGS_MIGEMO)
LDFLAGS = -nologo $(LDFLAGS_MIGEMO)
LIBS	=

LD = link.exe

rel: cmigemo.exe

cmigemo.exe: $(libmigemo_LIB) $(objdir)main.obj
	$(LD) $(LDFLAGS) -OUT:$@ $(libmigemo_LIB) $(LIBS) $(objdir)main.obj

migemo.res: migemo.rc resource.h
	rc.exe /fo $@ migemo.rc

$(libmigemo_LIB): $(libmigemo_DSO)
$(libmigemo_DSO): $(libmigemo_OBJ) migemo.def migemo.res
	$(LD) $(LDFLAGS) -OUT:$@ $(libmigemo_OBJ) $(LIBS) migemo.res -DLL -DEF:migemo.def -MAP:migemo.map

dictionary: cd-dict msvc
	cd ..

cd-dict:
	cd dict
