.PHONY: build
build: configure
	cmake --build build

.PHONY: configure
configure:
	cmake -B build

.PHONY: run
run:
	./build/Debug/Sudoku.exe

.PHONY: clean
clean:
	rm -rf build

.PHONY: copy
copy:
	cp -r ./build/Debug/Sudoku.exe ./Sudoku.exe