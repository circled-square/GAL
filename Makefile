all: build/debug/main
compile: build/debug/main
build/debug/main: src/* src/*/*
	cmake -DCMAKE_BUILD_TYPE=Debug -G "Unix Makefiles" -S src -B build/debug
	make -C build/debug/

compile_release: build/release/main
build/release/main: src/* src/*/*
	cmake -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" -S src -B build/release
	make -C build/release/


run: build/debug/main
	build/debug/main

run_software_mode: build/debug/main
	LIBGL_ALWAYS_SOFTWARE=1 build/debug/main

run_release: build/release/main
	build/release/main

run_release_software_mode: build/release/main
	LIBGL_ALWAYS_SOFTWARE=1 build/release/main


clean:
	rm build/* -fr
	rm imgui.ini -f
