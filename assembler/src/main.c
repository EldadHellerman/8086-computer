#include "assembler.h"
#include "stdbool.h"

/*
TODO:
    - have instructions and synonyms data sorted in source file so it is stored sorted in read only memory.
    - also, thing of a good data structure for them and implement it.
        (maybe one for assembly and for for dissasembly will be better).
*/

void sort_array_in_place(void *array, size_t array_length, size_t element_size, bool (*compare_function)(void *e1, void *e2)){
    char buffer[element_size];
    for(size_t already_sorted = 0; already_sorted < array_length; already_sorted++){
        for(size_t i = 0; i < array_length - already_sorted - 1; i++){
            void *ptr_first = array + i * element_size;
            void *ptr_second = array + (i + 1) * element_size;
            if(compare_function(ptr_first, ptr_second)){ // swap pair
                memcpy(buffer, ptr_first, element_size);
                memcpy(ptr_first, ptr_second, element_size);
                memcpy(ptr_second, buffer, element_size);
            }
        }
    }
}

bool compare_instructions(void *first_instruction, void *second_instruction){
    instruction_t *a = ((instruction_t *)first_instruction);
    instruction_t *b = ((instruction_t *)second_instruction);
    return (strcmp(a->mnemonic,b->mnemonic) == 1);
}

void print_instruction(instruction_t instruction){
    printf("%s:\n", instruction.mnemonic);
    for(uint8_t i = 0; i < instruction.formats_length; i++){
        if_t f = instruction.formats[i];
        printf("\tformat #%d - ", i);
        if(f.flags & IF_FLAG_V) printf("V, ");
        if(f.flags & IF_FLAG_D) printf("D, ");
        if(f.flags & IF_FLAG_S) printf("S, ");
        if(f.flags & IF_FLAG_W) printf("W, ");
        if(f.flags & IF_FLAG_AX_DST) printf("AX_DST, ");
        if(f.flags & IF_FLAG_AX_SRC) printf("AX_SRC, ");
        if(f.flags & IF_FLAG_DX) printf("DX, ");
        if(f.flags & IF_FLAG_REG) printf("REG, ");
        if(f.flags & IF_FLAG_SREG) printf("SREG, ");
        if(f.flags & IF_FLAG_MOD_RM) printf("MOD_RM, ");
        if(f.flags & IF_FLAG_DATA8) printf("DATA8, ");
        if(f.flags & IF_FLAG_DATA) printf("DATA, ");
        if(f.flags & IF_FLAG_ADDR16) printf("ADDR16, ");
        if(f.flags & IF_FLAG_SEG16) printf("SEG16, ");
        if(f.flags & IF_FLAG_DISP16) printf("DISP16, ");
        if(f.flags & IF_FLAG_DISP8) printf("DISP8, ");
        if(f.flags & IF_FLAG_ESC) printf("ESC, ");
        printf("\n");
    }

}

int main(int argv, char **argc){
    printf("Arguments:\n");
    for(int i=0; i < argv; i++) printf("\t#%d - '%s'\n", i+1, argc[i]);

    printf("hello! there are %d intrcutions and %d synonyms\n", instructions_length, instructions_synonyms_length);
    sort_array_in_place(instructions, instructions_length, sizeof(instruction_t), compare_instructions);
    
    for(int i = 0; i < instructions_length; i++) print_instruction(instructions[i]); // print all instructions
    for(int i = 0; i < instructions_synonyms_length; i++){ // print all synonyms
        instruction_synonym_t syn = instructions_synonyms[i];
        printf("%s -> %s\n", syn.synonym, syn.mnemonic);
    }
    return EXIT_SUCCESS;
}