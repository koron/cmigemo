# vim:set ts=8 sts=8 sw=8 tw=0 noet:

MIGEMO_DIR = ../src

TEST_SRC = \
	test1.c \
	main.c

MIGEMO_SRC = \
	$(MIGEMO_DIR)/charset.c \
	$(MIGEMO_DIR)/filename.c \
	$(MIGEMO_DIR)/migemo.c \
	$(MIGEMO_DIR)/mnode.c \
	$(MIGEMO_DIR)/romaji.c \
	$(MIGEMO_DIR)/rxgen.c \
	$(MIGEMO_DIR)/wordbuf.c \
	$(MIGEMO_DIR)/wordlist.c

CCFLAGS = -I. -I$(MIGEMO_DIR) -D_DEBUG

default: test

test: test_migemo
	@./test_migemo && echo OK || echo NG

clean:
	-rm -f *.o *.obj

distclean: clean
	-rm -f *.exe
	-rm -f test_migemo

test_migemo: $(TEST_SRC) $(MIGEMO_SRC)
	$(CC) $(CCFLAGS) -o $@ $(TEST_SRC) $(MIGEMO_SRC)
