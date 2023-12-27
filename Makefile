all: build/debug/src/main
compile: build/debug/src/main
build/debug/src/main: src/* src/*/*
	cmake -DCMAKE_BUILD_TYPE=Debug -G "Unix Makefiles" -S ./ -B build/debug
	make -C build/debug/

compile_release: build/release/src/main
build/release/src/main: src/* src/*/*
	cmake -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" -S ./ -B build/release
	make -C build/release/


run: build/debug/src/main
	build/debug/src/main

run_software_mode: build/debug/src/main
	LIBGL_ALWAYS_SOFTWARE=1 build/debug/src/main

run_release: build/release/src/main
	build/release/src/main

run_release_software_mode: build/release/src/main
	LIBGL_ALWAYS_SOFTWARE=1 build/release/src/main


clean:
	rm build/* -fr
	rm imgui.ini -f
