#include "assembler.h"
#include "tokenizer.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"

#include "print.h"

/* TODO:
    - have instructions data sorted in source file so it is stored sorted in read only memory.
    - also, think of a good data structure for them and implement it.
        (maybe one for assembly and for for dissasembly will be better).
    - maybe space can be saved by divinding instructions array to types:
        one_byte_instructions - ~30 instructions with one byte only.
        disp_instructions - ~20 all jump on comdition.
        complex_instructions - all instructions left, ~50.
    after doing that and checking: with all instructions as complex instructions, data size is ~2400 bytes.
        with the above seperation between instruction types, data size is ~1600 bytes.
    I will leave this as a step when optimizing for the 8086 itself to run the assembler.

    -what to do with: "push 0x12"? should it be "push word 0x12" or "pushw 0x12"?
    
    label tables can be store as array of chars, with each label having
    start index in table (until '\0'), and then it can even be sorted efficiently, and stored
    in a relativly compact form.

    - have a few spaces appended to file buffer, that way there is now need to account for last characters in file,
        using all thoes if(data_length > 2) checks (being careful with the space token itself).
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
    print("%c", (data & (1 << bit)) ? '1' : '0');
}

void print_instruction(instruction_t instruction){
    print("%s:\n", instruction.mnemonic);
    for(uint8_t i = 0; i < instruction.formats_length; i++){
        if_t f = instruction.formats[i];
        print("\t#%d - ", i+1);
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
        for(int8_t i = 7; i >= 0; i--) print("%c", output[i]);
        if((f.flags & IF_BYTES_MASK) == IF_BYTES_TWO){
            for(uint8_t i = 0; i < 8; i++) output[i] = (f.second_byte & (1 << i)) ? '1' : '0';
            print(" "); for(int8_t i = 7; i >= 0; i--) print("%c", output[i]);
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
            print(" "); for(int8_t i = 7; i >= 0; i--) print("%c", output[i]);
        }
        
        print(" ");
        // depending on addressing mode, optional (disp-low, disp-high) comes here
        if(f.flags & IF_SUFFIX_MASK){
            if((f.flags & IF_SUFFIX_MASK) == IF_SUFFIX_ADDR16) print("addr-low, addr-high");
            else if((f.flags & IF_SUFFIX_MASK) == IF_SUFFIX_OFFSET16_SEG16) print("offset-low, offset-high, seg-low, seg-high");
            else if((f.flags & IF_SUFFIX_MASK) == IF_SUFFIX_DISP16) print("disp-low, disp-high");
            else if((f.flags & IF_SUFFIX_MASK) == IF_SUFFIX_DATA8) print("data8, ");
            else if((f.flags & IF_SUFFIX_MASK) == IF_SUFFIX_DATA16) print("data-low, data-high, ");
            else if((f.flags & IF_SUFFIX_MASK) == IF_SUFFIX_DISP8) print("disp");
            else if((f.flags & IF_SUFFIX_MASK) == IF_SUFFIX_DATA_VW){
                if(f.flags & IF_DSV_S) print("data-low, data-high if sw=01");
                else print("data-low, data-high if w=1");
            }
        }
        if(f.flags & IF_IMPLICIT_MASK){
            print(" (");
            if((f.flags & IF_IMPLICIT_MASK) == IF_IMPLICIT_AX_SRC) print("AX_SRC, ");
            if((f.flags & IF_IMPLICIT_MASK) == IF_IMPLICIT_AX_DST) print("AX_DST, ");
            if((f.flags & IF_IMPLICIT_MASK) == IF_IMPLICIT_AX_BOTH) print("AX, ");
            if((f.flags & IF_IMPLICIT_MASK) == IF_IMPLICIT_AX_SRC_DX_DST) print("AX_SRC_DX_SRC, ");
            if((f.flags & IF_IMPLICIT_MASK) == IF_IMPLICIT_AX_DST_DX_SRC) print("AX_DST_DX_SRC, ");
            if((f.flags & IF_IMPLICIT_MASK) == IF_IMPLICIT_0x03) print("0x03, ");
            print(")");
        }
        print("\n");
    }

}

void print_all_instructions(){
    for(int i = 0; i < instructions_length; i++){
        print("%d - ", i);
        print_instruction(instructions[i]);
    }
}

void print_memory_usage(){
    int number_of_formats = 0;
    for(int i = 0; i < instructions_length; i++) number_of_formats += instructions[i].formats_length;
    int mem_instructions = (int)sizeof(instruction_t) * instructions_length;
    int mem_formats = (int)sizeof(if_t) * number_of_formats;
    log("memory use: instrcutions: %d, formats: %d\n", mem_instructions, mem_formats);
    log("total memory use: %d\n", mem_instructions + mem_formats);
}

/*void print_memory_usage_small(){
    int number_of_formats = 0;
    for(int i = 0; i < instructions_length; i++) number_of_formats += instructions[i].formats_length;
    int mem_instructions = (int)sizeof(instruction_t) * instructions_length;
    int mem_formats = (int)sizeof(if_t) * number_of_formats;
    int mem_simple_instructions = (int)sizeof(ifob_t) * instructions_byte_length;
    int mem_disp_instructions = (int)sizeof( ifob_t) * instructions_disp_length;
    log("memory use: instrcutions: %d, simple instructions: %d, disp instructions: %d, formats: %d\n",
        mem_instructions, mem_simple_instructions, mem_disp_instructions, mem_formats);
    log("total memory use: %d\n", mem_instructions + mem_simple_instructions + mem_disp_instructions + mem_formats); 
}*/

int main(int argv, char **argc){
    log("Arguments: \n");
    for(int i=0; i < argv; i++) print("\t#%d - '%s'\n", i+1, argc[i]);

    sort_array_in_place(instructions, instructions_length, sizeof(instruction_t), compare_instructions);
    
    print("there are %d intrcutions\n", instructions_length);
    // print_all_instructions();
    // print_memory_usage();
    return EXIT_SUCCESS;
}