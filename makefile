build-debug:
	@echo "Building debug..."
	@gcc -g -pedantic -std=c99 main.c -lcrypto -o main

build-release:
	@echo "Building release..."
	@$(MAKE) -f makefile.release

build: build-debug build-release
