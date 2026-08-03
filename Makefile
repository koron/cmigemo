# vi:set ts=8 sts=8 sw=8 tw=0:
#
# C/Migemo Makefile

.PHONY: build tags clean distclean

build:
	cmake -B build
	cmake --build build -- $(MAKEFLAGS)

tags: src/*.c src/*.h
	ctags src/*.c src/*.h

clean:
	@if [ -d build ]; then cmake --build build --target clean; fi

distclean:
	rm -rf build
	rm -f tags
