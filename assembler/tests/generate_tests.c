#include "assembler.h"
#include "tokenizer.h"
#include "print.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

const char *file_output = "build/test_basic.txt";

/** TODO:
 * - instead of writing to file, I can replace write functions with
 * immediate assembling and checking. (then i dont need to also read from file).
 * - then all possible far jumps with all 32-bit possibilties can be tested! (its too much writing them to file...).
 */

void write_with_data(FILE *fp, char *mnemonic, unsigned char first_byte, int from, int to, int number_of_bytes){
    for(int i = from; i < to; i++){
        fprintf(fp, "%s 0x%0*X\n" "\t0x%02X", mnemonic, number_of_bytes * 2, i, first_byte);
        int temp = i;
        for(int b = 0; b < number_of_bytes; b++){
            fprintf(fp, " 0x%02X", temp & 0xFF);
            temp >>= 8;
        }
        fprintf(fp, "\n");
    }
}

void write_with_segment(FILE *fp, char *mnemonic, unsigned char first_byte){
    int seg = 0;
    for(int s = 0; s < 0x10; s++){
        seg = s << 12;
        for(int i = 0; i < (2 << 15); i++){
            fprintf(fp, "%s 0x%04X:0x%04X\n" "\t0x%02X", mnemonic, seg, i, first_byte);
            fprintf(fp, " 0x%02X 0x%02X 0x%02X 0x%02X\n", i&0xFF, i>>8, seg&0xff, seg>>8);
        }
    }
}

void write_const(FILE *fp, char *mnemonic, unsigned char first_byte){
    fprintf(fp, "%s\n" "\t0x%02X\n", mnemonic, first_byte);
}

void write_const_two_bytes(FILE *fp, char *mnemonic, unsigned char first_byte, unsigned char second_byte){
    fprintf(fp, "%s\n" "\t0x%02X 0x%02X\n", mnemonic, first_byte, second_byte);
}



static inline void write_with_byte(FILE *fp, char *mnemonic, unsigned char first_byte){
    write_with_data(fp, mnemonic, first_byte, 0, 2<<7, 1);
}

static inline void write_with_two_bytes(FILE *fp, char *mnemonic, unsigned char first_byte){
    write_with_data(fp, mnemonic, first_byte, 0, 2<<15, 2);
}

void generate_move(FILE *fp){} // TODO

void generate_push_pop(FILE *fp){} // TODO

void generate_exchange(FILE *fp){} // TODO

void generate_in_out(FILE *fp){} // TODO

void generate_lea_lds_les(FILE *fp){} // TODO

void generate_add_adc(FILE *fp){} // TODO

void generate_inc(FILE *fp){} // TODO

void generate_sub_sbb(FILE *fp){} // TODO

void generate_dec(FILE *fp){} // TODO

void generate_neg(FILE *fp){} // TODO

void generate_cmp(FILE *fp){} // TODO

void generate_multiply(FILE *fp){} // TODO

void generate_divide(FILE *fp){} // TODO

void generate_logic(FILE *fp){
    // TODO all rotations, with CX and without, with word ptr and with byte ptr.
    // TODO for "AND", "TEST", "OR", "XOR"
}

void generate_string_manipulation(FILE *fp){
    write_const(fp, "REP", 0b11110011);
    write_const(fp, "REPE", 0b11110011);
    write_const(fp, "REPZ", 0b11110011);
    write_const(fp, "REPNE", 0b11110010);
    write_const(fp, "REPNZ", 0b11110010);
    write_const(fp, "MOVSB", 0b10100100);
    write_const(fp, "MOVSW", 0b10100101);
    write_const(fp, "CMPSB", 0b10100110);
    write_const(fp, "CMPSW", 0b10100111);
    write_const(fp, "SCASB", 0b10101110);
    write_const(fp, "SCASW", 0b10101111);
    write_const(fp, "LODSB", 0b10101100);
    write_const(fp, "LODSW", 0b10101101);
    write_const(fp, "STOSB", 0b10101010);
    write_const(fp, "STOSW", 0b10101011);
}

void generate_call(FILE *fp){
    write_with_two_bytes(fp, "CALL", 0b11101000);
    // TODO indirect within segment
    write_with_segment(fp, "CALLF", 0b10011010);
    // TODO indirect intersegment
}

void generate_jump(FILE *fp){
    write_with_data(fp, "JMP", 0b11101011, 0, 2<<7, 1);
    write_with_data(fp, "JMP", 0b11101001, 2<<7, 2<<15, 2);
    // TODO indirect within segment
    write_with_segment(fp, "JMPF", 0b11101010);
    // TODO indirect intersegment
}

void generate_return(FILE *fp){
    write_const(fp, "RET", 0b11000011);
    write_with_two_bytes(fp, "RET", 0b11000010);
    write_const(fp, "RETF", 0b11001011);
    write_with_two_bytes(fp, "RETF", 0b11001010);
}

void generate_jump_on_condition_and_loops(FILE *fp){
    write_with_byte(fp, "JE", 0b01110100); write_with_byte(fp, "JZ", 0b01110100);
    write_with_byte(fp, "JL", 0b01111100); write_with_byte(fp, "JNGE", 0b01111100);
    write_with_byte(fp, "JLE", 0b01111110); write_with_byte(fp, "JNG",0b01111110);
    write_with_byte(fp, "JB", 0b01110010); write_with_byte(fp, "JNAE",0b01110010);
    write_with_byte(fp, "JBE", 0b01110110); write_with_byte(fp, "JNA",0b01110110);
    write_with_byte(fp, "JP", 0b01111010); write_with_byte(fp, "JPE",0b01111010);
    write_with_byte(fp, "JO", 0b01110000);
    write_with_byte(fp, "JS", 0b01111000);
    
    write_with_byte(fp, "JNE", 0b01110101); write_with_byte(fp, "JNZ", 0b01110101);
    write_with_byte(fp, "JNL", 0b01111101); write_with_byte(fp, "JGE", 0b01111101);
    write_with_byte(fp, "JNLE", 0b01111111); write_with_byte(fp, "JG",0b01111111);
    write_with_byte(fp, "JNB", 0b01110011); write_with_byte(fp, "JAE",0b01110011);
    write_with_byte(fp, "JNBE", 0b01110111); write_with_byte(fp, "JA",0b01110111);
    write_with_byte(fp, "JNP", 0b01111011); write_with_byte(fp, "JPO",0b01111011);
    write_with_byte(fp, "JNO", 0b01110001);
    write_with_byte(fp, "JNS", 0b01111001);

    write_with_byte(fp, "LOOP", 0b11100010);
    write_with_byte(fp, "LOOPZ", 0b11100001); write_with_byte(fp, "LOOPE", 0b11100001);
    write_with_byte(fp, "LOOPNZ", 0b11100000); write_with_byte(fp, "LOOPNE", 0b11100000);
    write_with_byte(fp, "JCXZ", 0b11100011);
}

void generate_interrupts(FILE *fp){
    write_with_data(fp, "INT", 0b11001101, 0, 3, 1);
    write_const(fp, "INT 3", 0b11001100);
    write_with_data(fp, "INT", 0b11001101, 4, (2 << 7), 1);
    write_const(fp, "INTO", 0b11001110);
}

void generate_escape(FILE *fp){} // TODO

void generate_data_transfer(FILE *fp){
    generate_move(fp);
    generate_push_pop(fp);
    generate_exchange(fp);
    generate_in_out(fp);
    write_const(fp, "XLAT", 0b11010111);
    generate_lea_lds_les(fp);
    write_const(fp, "LAHF", 0b10011111);
    write_const(fp, "SAHF", 0b10011110);
    write_const(fp, "PUSHF", 0b10011100);
    write_const(fp, "POPF", 0b10011101);
}

void generate_arithmetic(FILE *fp){
    generate_add_adc(fp);
    generate_inc(fp);
    write_const(fp, "AAA", 0b00110111);
    write_const(fp, "BAA", 0b00100111);
    generate_sub_sbb(fp);
    generate_dec(fp);
    generate_neg(fp);
    generate_cmp(fp);
    write_const(fp, "AAS", 0b00111111);
    write_const(fp, "DAS", 0b00101111);
    generate_multiply(fp);
    write_const_two_bytes(fp, "AAM", 0b11010100, 0b00001010);
    generate_divide(fp);
    write_const_two_bytes(fp, "AAD", 0b11010101, 0b00001010);
    write_const(fp, "CBW", 0b10011000);
    write_const(fp, "CWD", 0b10011001);
}

void generate_control_transfer(FILE *fp){
    generate_call(fp);
    generate_jump(fp);
    generate_return(fp);
    generate_jump_on_condition_and_loops(fp);
    generate_interrupts(fp);
    write_const(fp, "IRET", 0b11001111);
}

void generate_processor_control(FILE *fp){
    write_const(fp, "CLC", 0b11111000);
    write_const(fp, "CMC", 0b11110101);
    write_const(fp, "STC", 0b11111001);
    write_const(fp, "CLD", 0b11111100);
    write_const(fp, "STD", 0b11111101);
    write_const(fp, "CLI", 0b11111010);
    write_const(fp, "STI", 0b11111011);
    write_const(fp, "HLT", 0b11110100);
    write_const(fp, "WAIT", 0b10011011);
    generate_escape(fp);
    write_const(fp, "LOCK", 0b11110000);
}

int main(void){
    log("Generating file...\n");
    FILE *fp = fopen(file_output, "wb");
    if(fp == NULL) exit(EXIT_FAILURE);
    
    generate_data_transfer(fp);
    generate_arithmetic(fp);
    generate_logic(fp);
    generate_string_manipulation(fp);
    generate_control_transfer(fp);
    generate_processor_control(fp);

    fclose(fp);
    return EXIT_SUCCESS;
}
