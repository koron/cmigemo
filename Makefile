# vim:set ts=8 sts=8 sw=8 tw=0 noet:
#
# C/Migemo Makefile

.PHONY: build test profile tags format package clean distclean

build:
	cmake -B build
	cmake --build build --parallel

tags: src/*.c src/*.h
	ctags src/*.c src/*.h

test:
	cmake -B build
	cmake --build build --parallel --target check

profile:
	cmake -B build
	cmake --build build --parallel --target profile

format:
	find src test include -type f \( -name "*.c" -o -name "*.h" \) | xargs clang-format -i

package:
	cmake -B build -DCMAKE_BUILD_TYPE=Release
	cmake --build build --target package --config Release --parallel

clean:
	@if [ -d build ]; then cmake --build build --target clean 2>/dev/null || true; fi

distclean:
	rm -rf build
	rm -f tags
