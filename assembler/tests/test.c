#include "assembler.h"
#include "tokenizer.h"
#include "print.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 255

enum state{
    ASSEMBLY,
    BINARY,
    EMPTY
};
enum state state = ASSEMBLY;

bool test_assembler_basic(){
    bool result = true;
    log("testing basic assembly commands from file.\n");
    FILE* fp;
    char buffer_line_assebly[BUFFER_SIZE];
    char buffer_line_binary[BUFFER_SIZE];
    unsigned char buffer_binary[BUFFER_SIZE];
    int buffer_binary_size;
    char *hex_end;

    fp = fopen("tests/instructions_and_binary.txt", "r");
    if (fp == NULL) exit(EXIT_FAILURE);
    while(fgets(buffer_line_assebly, BUFFER_SIZE, fp)) {
        size_t buffer_line_assembly_size = strlen(buffer_line_assebly);
        if(buffer_line_assembly_size == 1) continue; // empty line. skip to next one.
        if(!fgets(buffer_line_binary, BUFFER_SIZE, fp)){
            error("There is an assembly line without a matching binary line.\n");
            exit(EXIT_FAILURE);
        }
        size_t buffer_line_binary_size = strlen(buffer_line_binary);

        buffer_line_assebly[buffer_line_assembly_size- 1] = 0; // remove trailing '\n'
        buffer_line_binary[buffer_line_binary_size - 1] = 0; // remove trailing '\n'
        print("'%s'   ->   ", buffer_line_assebly);
        
        buffer_binary_size = 0;
        hex_end = &buffer_line_binary[0];
        while(1){
            char * current_index = hex_end;
            long l = strtol(hex_end, &hex_end, 0);
            if(current_index == hex_end) break;
            buffer_binary[buffer_binary_size] = l;
            buffer_binary_size++;
        }
        for(int i = 0; i < buffer_binary_size; i++){
            print("%02X, ", buffer_binary[i]);
        }
        print("\n");
        if(0){
            result = false;
            error("Expecting: but got: ...\n");
        }
    }
    fclose(fp);
    return result;
}


const char *file_path_tokenizer = "tests/programs/prog1.asm";

bool test_tokenizer(){
    bool result = true;
    log("Testing tokenizer from file %s.\n", file_path_tokenizer);
    FILE* fp;
    char *buffer;
    unsigned int file_size;

    fp = fopen(file_path_tokenizer, "rb");
    if (fp == NULL){
        error("Could not open file.\n");
        exit(EXIT_FAILURE);
    }
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    buffer = malloc(file_size);
    if (buffer == NULL) exit(EXIT_FAILURE);
    unsigned int read = (unsigned int)fread(buffer, 1, file_size, fp);
    if(read != file_size){
        error("File was not read completely. Read %d bytes out of %d.\n", read, file_size);
        free(buffer);
        exit(EXIT_FAILURE);
    }
    fclose(fp);

    unsigned int bytes_remainging = file_size;
    char *data = buffer;
    while(1){
        /*
        char temp[40] = {0};
        int ti = 0;
        for(int i=0; i<20; i++){
            if(data[i] == '\n'){ temp[ti++] = '\\'; temp[ti++] = 'n'; break;}
            else if(data[i] == '\r'){ temp[ti++] = '\\'; temp[ti++] = 'r';}
            else temp[ti++] = data[i]; 
        }
        print("get_token(\"%-15s\")   -   ", temp);
        */

        token t = get_token(data, bytes_remainging);
        if(t.type != SPACE){
            print_token(t);
            if(t.type == LABEL) print(" ('%.*s')", t.number_of_characters-1, data+1);
            if(t.type == OTHER) print(" ('%.*s')", t.number_of_characters, data);
            if(t.type == NEWLINE) print("\n"); else print(", ");
        }

        data += t.number_of_characters;
        bytes_remainging -= t.number_of_characters;
        if(t.type == END_OF_BUFFER) break;
        if(t.number_of_characters == 0){
            data++;
            bytes_remainging--;
        }
    }
    print("\n-------   end   -------\n");
    free(buffer);
    return result;
}

int main(void){
    log("Running tests...\n");
    bool result = true;
    // result &= test_assembler_basic();
    result &= test_tokenizer();
    if(result){
        log(COLOR_GREEN "Passed all tests!\n" COLOR_END);
    }else{
        warning("Some tests failed.\n");
    }
    return EXIT_SUCCESS;
}