.PHONY: test

main:
	gcc -Wall -Wextra -Iinclude src/main.c src/assembler.c src/isa.c -o build/as.exe
	./build/as.exe

test:
	gcc -Wall -Wextra -Iinclude test/test.c src/assembler.c src/isa.c -o build/test.exe
	./build/test.exe