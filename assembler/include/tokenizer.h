#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "isa.h"

/**
 * @brief
 * 
 * Token types are in order of precedence, meaning, for example,
 * that mnemonics are checked for a match before labels.
 */
typedef enum{
    // one character:
    COMMA,
    COLON,
    SEMICOLON,
    NEWLINE, // '\n' or '\r\n'
    BRACKET_SQUARE_LEFT,
    BRACKET_SQUARE_RIGHT,
    OPERATOR_PLUS,
    OPERATOR_MINOS,
    CURRENT_ADDRESS,

    // advanced expressions stuff:
    // OPERATOR_MULTIPLICATION,
    // OPERATOR_DIVISION,
    // OPERATOR_REMAINDER,
    // OPERATOR_GREATER_THAN,
    // OPERATOR_LESS_THAN,
    // OPERATOR_BITWISE_AND,
    // OPERATOR_BITWISE_OR,
    // OPERATOR_BITWISE_XOR,
    // OPERATOR_BITWISE_NOT,
    // OPERATOR_LOGICAL_NOT,

    // two characters:
    // OPERATOR_GREATER_THAN_EQUAL,
    // OPERATOR_LESS_THAN_EQUAL,
    // OPERATOR_LOGICAL_AND,
    // OPERATOR_LOGICAL_OR,
    // OPERATOR_LOGICAL_XOR,
    // OPERATOR_SHIFT_LEFT,
    // OPERATOR_SHIFT_RIGHT,
    
    // four characters:
    POINTER_WORD, // "word"
    POINTER_BYTE, // "byte"
    
    // variable length:
    SPACE, // one or more spaces
    COMMENT_SINGLE_LINE, // '//', up to new line token
    COMMENT_MULTI_LINE, // from '/*' up to and including '*/'
    COMMENT_MULTI_LINE_INVALID, // from '/*' up to and including '*/'
    CONSTANT_INTEGER_DECIMAL,
    CONSTANT_INTEGER_BINARY,
    CONSTANT_INTEGER_HEX,
    CONSTANT_INTEGER_OCTAL,
    CONSTANT_FLOAT,
    CONSTANT_CHAR,
    CONSTANT_CHAR_ESCAPED,
    CONSTANT_STRING,
    CONSTANT_STRING_INVALID,
    LABEL, // label name
    // index in array is stored in type.index:
    DIRECTIVE,
    REGISTER_BYTE,
    REGISTER_WORD,
    REGISTER_SEGMENT,
    MNEMONIC,
    
    // other:
    END_OF_BUFFER,
    OTHER,
    UNDETERMINED
} token_type;



typedef struct{ // TODO this struct can probably fit in 2 bytes.
    token_type type;
    /** Index of directive, mnemonic, or register, depending on type.
     * for some types this can be 0.
     */
    unsigned char index;
    /** How many characters this token takes.
     * Useful for incrementing pointer through text, and to get strings when token is of type other (when expecting strings).
     */
    unsigned char number_of_characters;
} token;

token get_token(const char *data, unsigned int data_length);
void print_token(token t);

#endif /* TOKENIZER_H */