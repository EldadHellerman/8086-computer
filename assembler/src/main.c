#include "assembler.h"
#include "stdbool.h"

/* TODO:
    - have instructions and synonyms data sorted in source file so it is stored sorted in read only memory.
    - also, thing of a good data structure for them and implement it.
        (maybe one for assembly and for for dissasembly will be better).
    - maybe space can be saved by divinding instructions array to types:
        one_byte_instructions - ~30 instructions with one byte only.
        disp_instructions - ~20 all jump on comdition. then it will also save space to store synonym instructions and remove them from synonms array
        complex_instructions - all instructions left, ~50.
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

void print_bit(uint8_t data, uint8_t bit){
    printf("%c", (data & (1 << bit)) ? '1' : '0');
}

void print_instruction(instruction_t instruction){
    printf("%s:\n", instruction.mnemonic);
    for(uint8_t i = 0; i < instruction.formats_length; i++){
        if_t f = instruction.formats[i];
        printf("\t#%d - ", i+1);
        char output[9] = {[8] = '\0'}; // char output of byte
        for(uint8_t i = 0; i < 8; i++) output[i] = (f.first_byte & (1 << i)) ? '1' : '0';
        if((f.flags & IF_FLAG_REG) && !(f.flags & IF_BYTES_MOD_RM)){
            output[IF_FB_REG_BIT + 2] =  'R';
            output[IF_FB_REG_BIT + 1] =  'E';
            output[IF_FB_REG_BIT] =  'G';
        }
        if(f.flags & IF_FLAG_W){
            if((f.flags & IF_FLAG_REG) && !(f.flags & IF_BYTES_MOD_RM)){
                output[IF_FB_W_WITH_REG_BIT] =  'W';        
            }else{
                output[IF_FB_W_WITHOUT_REG_BIT] =  'W';
            }
        }
        if(f.flags & IF_FLAG_SREG && !(f.flags & IF_BYTES_MOD_RM)){
            output[IF_FB_SREG_BIT + 1] =  'S';
            output[IF_FB_SREG_BIT] =  'R';
        }
        if((f.flags & IF_DSV_MASK) == IF_DSV_D) output[IF_FB_D_BIT] =  'D';
        else if((f.flags & IF_DSV_MASK) == IF_DSV_S) output[IF_FB_S_BIT] =  'S';
        else if((f.flags & IF_DSV_MASK) == IF_DSV_V) output[IF_FB_V_BIT] =  'V';

        if(f.flags & IF_FLAG_ESC){
            output[IF_FB_ESC_BIT + 2] =  'x';
            output[IF_FB_ESC_BIT + 1] =  'x';
            output[IF_FB_ESC_BIT] =  'x';
        }
        for(int8_t i = 7; i >= 0; i--) printf("%c", output[i]);
        if((f.flags & IF_BYTES_MASK) == IF_BYTES_TWO){
            for(uint8_t i = 0; i < 8; i++) output[i] = (f.second_byte & (1 << i)) ? '1' : '0';
            printf(" "); for(int8_t i = 7; i >= 0; i--) printf("%c", output[i]);
        }else if((f.flags & IF_BYTES_MASK) == IF_BYTES_MOD_RM){
            for(uint8_t i = 0; i < 8; i++) output[i] = (f.second_byte & (1 << i)) ? '1' : '0';
            output[7] = 'M';
            output[6] = 'D';
            output[2] = 'R';
            output[1] = '/';
            output[0] = 'M';
            if(f.flags & IF_FLAG_ESC){
                output[IF_SB_ESC_BIT + 2] =  'y';
                output[IF_SB_ESC_BIT + 1] =  'y';
               output[IF_SB_ESC_BIT] =  'y';
            }
            if(f.flags & IF_FLAG_REG){
                output[IF_SB_REG_BIT + 2] =  'R';
                output[IF_SB_REG_BIT + 1] =  'E';
               output[IF_SB_REG_BIT] =  'G';
            }
            if(f.flags & IF_FLAG_SREG){
                output[IF_SB_SREG_BIT + 1] =  'S';
               output[IF_SB_SREG_BIT] =  'R';
            }
            printf(" "); for(int8_t i = 7; i >= 0; i--) printf("%c", output[i]);
        }
        
        printf(" ");
        // depending on addressing mode, optional (disp-low, disp-high) comes here
        if(f.flags & IF_FLAG_ADDR16) printf("addr-low, addr-high");
        if(f.flags & IF_FLAG_SEG16) printf("seg-low, seg-high");
        if(f.flags & IF_FLAG_DISP16) printf("disp-low, disp-high");
        if(f.flags & IF_FLAG_DATA){
            if(f.flags & IF_FLAG_W){
                if(f.flags & IF_DSV_S){
                    printf("data-low, data-high if sw=01");
                }else{
                    printf("data-low, data-high if w=1");
                }
            }else{
                printf("data-low, data-high");
            }
        }
        if(f.flags & IF_FLAG_DATA8) printf("data8, ");
        if(f.flags & IF_FLAG_DISP8) printf("disp");
        if(f.flags & IF_IMPLICIT_MASK){
            printf(" (");
            if((f.flags & IF_IMPLICIT_MASK) == IF_IMPLICIT_AX_SRC) printf("AX_SRC, ");
            if((f.flags & IF_IMPLICIT_MASK) == IF_IMPLICIT_AX_DST) printf("AX_DST, ");
            if((f.flags & IF_IMPLICIT_MASK) == IF_IMPLICIT_AX_BOTH) printf("AX, ");
            if((f.flags & IF_IMPLICIT_MASK) == IF_IMPLICIT_DX_SRC) printf("DX_SRC, ");
            if((f.flags & IF_IMPLICIT_MASK) == IF_IMPLICIT_DX_DST) printf("DX_DST, ");
            if((f.flags & IF_IMPLICIT_MASK) == IF_IMPLICIT_0x03) printf("0x03, ");
            printf(")");
        }
        printf("\n");
    }

}

void print_all_instructions(){
    for(int i = 0; i < instructions_length; i++) print_instruction(instructions[i]);
}

void print_all_synonyms(){
    for(int i = 0; i < instructions_synonyms_length; i++){
        instruction_synonym_t syn = instructions_synonyms[i];
        printf("%s -> %s\n", syn.synonym, syn.mnemonic);
    }
}

int main(int argv, char **argc){
    printf("Arguments:\n");
    for(int i=0; i < argv; i++) printf("\t#%d - '%s'\n", i+1, argc[i]);

    sort_array_in_place(instructions, instructions_length, sizeof(instruction_t), compare_instructions);
    
    // print_all_instructions();
    // print_all_synonyms();

    printf("there are %d intrcutions and %d synonyms\n", instructions_length, instructions_synonyms_length);
    int number_of_formats = 0;
    for(int i = 0; i < instructions_length; i++) number_of_formats += instructions[i].formats_length;
    int mem_instructions = (int)sizeof(instruction_t) * instructions_length;
    int mem_formats = (int)sizeof(if_t) * number_of_formats;
    int mem_synonyms = (int)sizeof(instruction_synonym_t) * instructions_synonyms_length;
    printf("memory use: instrcutions: %d, formats: %d synonyms %d\n",
        mem_instructions, mem_formats, mem_synonyms);
    printf("total memory use: %d\n", mem_instructions + mem_formats + mem_synonyms);
    return EXIT_SUCCESS;
}