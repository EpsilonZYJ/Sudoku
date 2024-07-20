.PHONY: build
build: configure
	cmake --build build

.PHONY: configure
configure:
	cmake -B build

.PHONY: run
run:
	./build/Sudoku

.PHONY: clean
clean:
	rm -rf build
