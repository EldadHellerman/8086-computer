#ifndef ASSEMBLER_H
#define ASSEMBLER_H
#include <stdio.h>
#include <inttypes.h>

// Directives:
extern const char *strings_directives[];
extern uint8_t strings_directives_length;

// unsigned int assemble(FILE *file_input, FILE *file_output);
unsigned int assemble(FILE *file_input);
#endif /* ASSEMBLER_H */