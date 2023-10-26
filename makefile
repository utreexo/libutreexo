CC=gcc

all: build-relese

build-debug:
	@echo "Building debug..."
	@gcc -g -D DEBUG -pedantic -std=c99 src/forest_test.c -lcrypto -o main

build-release:
	@echo "Building release..."
	@$(MAKE) -f makefile.release

tests:
	@echo "Building tests..."
	@gcc -g -pedantic -std=c99 src/forest_test.c -lcrypto -o forest_tests
	@gcc -g -pedantic -std=c99 src/test_flat_file.c -lcrypto -o flat_file_tests
	@./flat_file_tests
	@./forest_tests
clean: 
	@echo "Cleaning up..."
	@rm forest_tests *.bin flat_file*

build: build-debug
