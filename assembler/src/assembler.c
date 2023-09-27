#include "isa.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

int main(int argv, char **argc){
    printf("hello! there are %d intrcutions, and %d registers\n",
        number_of_instructions, number_of_registers);

    
    // copy instruction to inst_sorted and then sort them in place by mnemonic:
    instruction inst_sorted[number_of_instructions];
    for(int i = 0; i < number_of_instructions; i++) inst_sorted[i] = instructions[i];
    for(int already_sorted = 0; already_sorted < number_of_instructions; already_sorted++){
        for(int i = 0; i < number_of_instructions - already_sorted - 1; i++){
            if(strcmp(inst_sorted[i].mnemonic, inst_sorted[i+1].mnemonic) == 1){ //swap pair
                instruction temp = inst_sorted[i];
                inst_sorted[i] = inst_sorted[i+1];
                inst_sorted[i+1] = temp;
            }
        }
    }
    
    for(int i = 0; i < number_of_instructions; i++){
        instruction inst = inst_sorted[i];
        printf("%s (%d) - 0x%02x\n", inst.mnemonic, inst.size, inst.b);
    }
    return EXIT_SUCCESS;
}