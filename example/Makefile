defalut: build

cmake:
	cmake -S  . -B Build \
		-DCMAKE_BUILD_TYPE=Release \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=1

build: cmake
	cd Build && make

run:
	./Build/PdfsmExample

clean:
	make -C Build clean

.PHONY: build
