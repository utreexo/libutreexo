CC=gcc
CFLAGS=-Wall -We -g -pedantic -std=c99

all: build-release

build-debug:
	@echo "Building debug..."
	@gcc -g -D DEBUG $(CFLAGS) src/forest_test.c -lcrypto -o main

build-release:
	@echo "Building release..."
	@$(CC) src/mmap_forest.c -c -o libforest.o
	@ar rcs libforest.a libforest.o
	@$(CC) src/mmap_forest.c -o libforest.so -fPIC -rdynamic -shared

tests:
	@echo "Building tests..."
	@gcc $(CFLAGS) src/forest_test.c -lcrypto -o forest_tests
	@gcc $(CFLAGS) src/test_flat_file.c -lcrypto -o flat_file_tests

test: tests
	@./flat_file_tests
	@./forest_tests

clean: 
	@echo "Cleaning up..."
	@rm forest_tests *.bin flat_file*

build: build-debug
