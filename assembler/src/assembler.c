#include "assembler.h"
#include "tokenizer.h"
#include "print.h"
#include "isa.h"
#include <stdlib.h>
#include <string.h>

#define BUFFER_LINE_SIZE 255

// Directives:

const char *strings_directives[] = {
    ".SECTION", // .section  SECTION_NAME
    ".GLOBAL", // .global LABEL_NAME
    ".EXTERN", // .extern LABEL_NAME, or like gcc, every not defiend label is extern by default
    ".ENTRY", // .entry  LABEL_NAME, or just .entry for current location
    ".TIMES", // .times number ... \n
    ".EQU", // .equ identifier expression, like ".equ msg_len $-message"

    ".INCBIN", // .incbin "FILE_PATH"
    ".ALIGN", // .align number
    ".DB", // .db list of 8-bit integers, chars, or strings
    ".DW", // .dw list of 16-bit integers.
    ".DD", // .dd list of 32-bit integers, or floating point numbers.
    ".DQ", // .dq list of 64-bit integers, or double floating point numbers.

    ".ORG", // .org origin_address
    ".RESB", // .resb number_of_1_bytes
    ".RESW", // .resw number_of_2_bytes
    ".RESD", // .resd number_of_4_bytes
    ".RESQ", // .resq number_of_8_bytes
    
};
uint8_t strings_directives_length = sizeof(strings_directives) / sizeof(char*);

unsigned int line_counter;

// TODO write function get_operand using get_tokens.

/*while(1){
    token t = get_token();
    if(t.type == END_OF_BUFFER) break;
    uint8_t data[4];
    // ignore spaces, increment line counter on newline, stop at end of buffer.
    // $label: | [REP, LOCK] instruction | directive | preprocessor directive

}*/

unsigned int assemble_part(char *data, unsigned int data_length){
    unsigned int bytes_remainging = data_length;
    while(1){
        token t = get_token(data, bytes_remainging);
        data += t.number_of_characters;
        bytes_remainging -= t.number_of_characters;
        if(t.type == END_OF_BUFFER) break;
        if(t.number_of_characters == 0){
            error("im here...\n");
            data++;
            bytes_remainging--;
        }
        if(t.type == COMMENT_MULTI_LINE || t.type == COMMENT_SINGLE_LINE || t.type == SPACE){
            continue;
        }
        if(t.type == NEWLINE){
            line_counter++;
            print("\n#% 3d:   ", line_counter);
            continue;
        }

        print_token(t);
        if(t.type == LABEL) print(" ('%.*s')", t.number_of_characters-1, data+1);
        if(t.type == OTHER) print(" ('%.*s')", t.number_of_characters, data);
        print("   ");
    }
    return data_length - bytes_remainging;
}

unsigned int assemble(FILE *file_input){
    log("Assembling:\n");
    char buffer[BUFFER_LINE_SIZE];
    line_counter = 1;
    // cannot handle '\0' in source...
    unsigned int bytes_read;
    print("#% 3d:   ", line_counter);
    while(fgets(buffer, sizeof(buffer), file_input) != NULL){
        unsigned int read = assemble_part(buffer, strlen(buffer));
        bytes_read += read;
    }
    print("\n-------   end   -------\n");
    log("Read %d bytes.\n", bytes_read);
    return 0;
}