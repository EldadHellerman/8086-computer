#include "assembler.h"
/*
TODO:
 - add tests for ESC instruction.
 - add tests for instructions above jump on condition instructions.
*/

#define BUFFER_SIZE 255

enum state{
    ASSEMBLY,
    BINARY,
    EMPTY
};
enum state state = ASSEMBLY;

bool test_assembler_basic(){
    bool result = true;
    printf("testing basic assembly commands from file.\n");
    FILE* fp;
    char buffer_line_assebly[BUFFER_SIZE];
    char buffer_line_binary[BUFFER_SIZE];
    unsigned char buffer_binary[BUFFER_SIZE];
    int buffer_binary_size;
    char *hex_end;

    fp = fopen("test/instructions_and_binary.txt", "r");
    if (fp == NULL) exit(EXIT_FAILURE);
    while(fgets(buffer_line_assebly, BUFFER_SIZE, fp)) {
        size_t buffer_line_assembly_size = strlen(buffer_line_assebly);
        if(buffer_line_assembly_size == 1) continue; // empty line. skip to next one.
        if(!fgets(buffer_line_binary, BUFFER_SIZE, fp)){
            fprintf(stderr, "There is an assembly line without a matching binary line.\n");
            exit(EXIT_FAILURE);
        }
        size_t buffer_line_binary_size = strlen(buffer_line_binary);

        buffer_line_assebly[buffer_line_assembly_size- 1] = 0; // remove trailing '\n'
        buffer_line_binary[buffer_line_binary_size - 1] = 0; // remove trailing '\n'
        printf("'%s'   ->   ", buffer_line_assebly);
        
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
            printf("%02X, ", buffer_binary[i]);
        }
        printf("\n");
        if(0){
            result = false;
            printf("Expecting: but got: ...\n");
        }
    }
    fclose(fp);
    return result;
}


void generate_jc(FILE *fp, char *mnemonic, unsigned char first_byte){
    for(int i = 0; i<256; i++){
        fprintf(fp, "%s 0x%02X\n\t0x%02X 0x%02X\n", mnemonic, i, first_byte, i);
    }
    fprintf(fp, "\n");
}

void generate_jump_conditional_to_file(){
    FILE *fp = fopen("result.txt", "wb");
    if(fp == NULL) exit(EXIT_FAILURE);
    generate_jc(fp, "JE", 0b01110100); generate_jc(fp, "JZ", 0b01110100);
    generate_jc(fp, "JL", 0b01111100); generate_jc(fp, "JNGE", 0b01111100);
    generate_jc(fp, "JLE", 0b01111110); generate_jc(fp, "JNG",0b01111110);
    generate_jc(fp, "JB", 0b01110010); generate_jc(fp, "JNAE",0b01110010);
    generate_jc(fp, "JBE", 0b01110110); generate_jc(fp, "JNA",0b01110110);
    generate_jc(fp, "JP", 0b01111010); generate_jc(fp, "JPE",0b01111010);
    generate_jc(fp, "JO", 0b01110000);
    generate_jc(fp, "JS", 0b01111000);
    
    generate_jc(fp, "JNE", 0b01110101); generate_jc(fp, "JNZ", 0b01110101);
    generate_jc(fp, "JNL", 0b01111101); generate_jc(fp, "JGE", 0b01111101);
    generate_jc(fp, "JNLE", 0b01111111); generate_jc(fp, "JG",0b01111111);
    generate_jc(fp, "JNB", 0b01110011); generate_jc(fp, "JAE",0b01110011);
    generate_jc(fp, "JNBE", 0b01110111); generate_jc(fp, "JA",0b01110111);
    generate_jc(fp, "JNP", 0b01111011); generate_jc(fp, "JPO",0b01111011);
    generate_jc(fp, "JNO", 0b01110001);
    generate_jc(fp, "JNS", 0b01111001);

    generate_jc(fp, "LOOP", 0b11100010);
    generate_jc(fp, "LOOPZ", 0b11100001); generate_jc(fp, "LOOPE", 0b11100001);
    generate_jc(fp, "LOOPNZ", 0b11100000); generate_jc(fp, "LOOPNE", 0b11100000);
    generate_jc(fp, "JCXZ", 0b11100011);
    fclose(fp);
}

int main(void){
    bool result = true;
    result &= test_assembler_basic();
    if(result){
        printf("Passed all tests!\n");
    }else{
        printf("Some tests failed.\n");
    }
    generate_jump_conditional_to_file();
    return EXIT_SUCCESS;
}