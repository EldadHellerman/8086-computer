CFLAGS = -Wall -Wextra -Iinclude
# files without main or test, only assembler files:
FILES = src/assembler.c src/isa.c src/tokenizer.c src/preprocessor.c
OUTPUT = build/as.exe

.PHONY: debug build test

build:
	clear
	gcc $(CFLAGS) src/main.c $(FILES) -o $(OUTPUT)
	@timeout 1
	./$(OUTPUT) tests/programs/prog3.asm

debug:
	clear
	gcc -g $(CFLAGS) src/main.c $(FILES) -o $(OUTPUT)

test:
	clear
	gcc $(CFLAGS) tests/test.c $(FILES) -o build/test.exe
	./build/test.exe

generate_tests:
	clear
	gcc $(CFLAGS) tests/generate_tests.c $(FILES) -o build/generate_tests.exe
	./build/generate_tests.exe
