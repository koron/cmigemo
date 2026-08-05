# vim:set ts=8 sts=8 sw=8 tw=0 noet:
#
# C/Migemo Makefile

.PHONY: build tags format package clean distclean

build:
	cmake -B build
	cmake --build build --parallel

tags: src/*.c src/*.h
	ctags src/*.c src/*.h

format:
	find . -type f \( -name "*.c" -o -name "*.h" \) | xargs clang-format -i

package:
	cmake -B build -DCMAKE_BUILD_TYPE=Release
	cmake --build build --target package --config Release --parallel

clean:
	@if [ -d build ]; then cmake --build build --target clean 2>/dev/null || true; fi

distclean:
	rm -rf build
	rm -f tags
