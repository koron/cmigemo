# vim:set ts=8 sts=8 sw=8 tw=0 noet:

DICTDIR = ../../dict2/utf-8.d

OUTPUT = qspeed romaji

MIGEMO_SRC = ../charset.c ../filename.c ../migemo.c ../mnode.c ../romaji.c \
	     ../rxgen.c ../wordbuf.c ../wordlist.c
ROMAJI_SRC = romaji_main.c ../romaji.c ../wordbuf.c ../charset.c
QSPEED_SRC = profile_speed.c $(MIGEMO_SRC)

CCFLAGS = -I. -I.. -DDICTDIR=\"$(DICTDIR)\" -D_DEBUG
CCFLAGS_PROF = -pg

default: all

all: romaji qspeed

qspeed: $(QSPEED_SRC)
	$(CC) $(CCFLAGS) $(CCFLAGS_PROF) -o $@ $(QSPEED_SRC)

romaji: $(ROMAJI_SRC)
	$(CC) $(CCFLAGS) -o $@ $(ROMAJI_SRC)

clean:
	rm -f gmon.out *.log

distclean: clean
	rm -f *.obj *.o *.exe
	rm -f $(OUTPUT)

profile: profile.log

profile.log: qspeed gmon.out
	if [ -f qspeed.exe ] ; then \
	  gprof qspeed.exe gmon.out > $@ ; \
	else \
	  gprof qspeed gmon.out > $@ ; \
	fi

gmon.out: qspeed
	./qspeed
	if [ -f qspeed.gmon ] ; then mv qspeed.gmon gmon.out ; fi
