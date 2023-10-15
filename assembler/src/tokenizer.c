#include "tokenizer.h"
#include "isa.h"
#include "assembler.h"
#include "string.h"
#include "stdio.h"
#include "stdbool.h"

/**
 * TODO
 *  - char doesn't support excape characters like '\0'
 *  - float is not detected correctly (plus or minus allowed after 'e')
 * 
 */

#define NO_MATCH 0xFF
#define INVALID_IDENTIFIER 0xFF

#define lower_case(c) (c | 0x20)
#define valid_identifier_char_first(c)      (('a' <= lower_case(c) && lower_case(c) <= 'z') || c == '_')
#define valid_identifier_char(c)            (valid_identifier_char_first(c) || ('0' <= c && c <= '9'))

#define valid_digit_decimal(c)              ('0' <= c && c <= '9')
#define valid_digit_binary(c)               ('0' <= c && c <= '1')
#define valid_digit_octal(c)                ('0' <= c && c <= '7')
#define valid_digit_hex(c)                  (('0' <= c && c <= '9') || ('a' <= lower_case(c) && lower_case(c) <= 'f'))
#define valid_digit_float(c)                (('0' <= c && c <= '9') || lower_case(c) == 'e' || c == '.')


static token get_simple_token(const char *data, unsigned int data_length);


/**
 * @brief 
 * if limit is reached, false is returned!
 * @param str 
 * @param start 
 * @param limit 
 * @return true 
 * @return false 
 */
static bool starts_with(const char *str, const char* start, unsigned char limit){
    while((*start != '\0') && (limit > 0)){
        if(lower_case(*str) != lower_case(*start)) return false;
        start++;
        str++;
        limit--;
    }
    return (get_simple_token(str, limit).type != UNDETERMINED);
}

static inline unsigned char get_identifier_length(const char *data, unsigned int data_length){
    unsigned char result = 0;
    if(data_length < 1 || !valid_identifier_char_first(data[0])) return result;
    result++;
    while(data_length > result && valid_identifier_char(data[result])) result++;
    // identifier must end with an invalid identifer character (or else maybe its just the end of the buffer)
    if(data_length > result && !valid_identifier_char(data[result])) return result;
    else return INVALID_IDENTIFIER;
}

/**
 * @brief 
 * 
 * @param data 
 * @param data_length 
 * @param array string filed of the first struct in the struct array.
 * @param array_length number of structs in array.
 * @param struct_size size of struct.
 * @return unsigned char 
 */
static unsigned char search_array(const char *data, unsigned int data_length, const char **array, unsigned char array_length){
    for(unsigned char index = 0; index < array_length; index++){
        if(starts_with(data, *array, data_length)) return index;
        array++;
    }
    return NO_MATCH;
}

static unsigned char search_mnemonic_array(const char *data, unsigned int data_length){
    for(unsigned char index = 0; index < instructions_length; index++){
        if(starts_with(data, instructions[index].mnemonic, data_length)) return index;
    }
    return NO_MATCH;
}

// bool is_valid_identifier_char(char character){
//     char c = character | 0x20; // lower case
//     return (('a' < c && c < 'z') || c == '_');
// }

static unsigned char is_newline(const char *data, unsigned int data_length){
    if(data_length >= 1 && data[0] == '\n') return 1;
    else if(data_length >= 2 && data[0] == '\r' && data[1] == '\n') return 2;
    return 0;
}

/**
 * @brief Get simple token object
 * Simple tokens are tokens which can be fully determined,
 * irrelavnt of their context.
 * That also means that they can be used to seperate other tokens.
 * For example: "$label_1+0x34" is equivalent to "$label_1 + 0x34",
 * since '+' is always used for the token PLUS, and nothing else.
 * 
 * @param data
 * @param data_length
 * @return token
 */
static token get_simple_token(const char *data, unsigned int data_length){
    token result = {.type = UNDETERMINED, .index = 0, .number_of_characters = 0};

    if(data_length < 1){
        result.type = END_OF_BUFFER;
        return result;
    }

    // try to match spaces, variable length:
    if(data[0] == ' '){
        result.type = SPACE;
        result.number_of_characters = 1;
        while(data_length > 0 && data[1] == ' '){
            result.number_of_characters++;
            data++;
            data_length--;
        }
        return result;
    }

    // try to match a single character:
    switch(data[0]){
        case ',':
            result.type = COMMA;
            break;
        case ':':
            result.type = COLON;
            break;
        case ';':
            result.type = SEMICOLON;
            break;
        case '[':
            result.type = BRACKET_SQUARE_LEFT;
            break;
        case ']':
            result.type = BRACKET_SQUARE_RIGHT;
            break;
        case '+':
            result.type = OPERATOR_PLUS;
            break;
        case '-':
            result.type = OPERATOR_MINOS;
            break;
    }
    if(result.type != UNDETERMINED){
        result.number_of_characters = 1;
        return result;
    }
    
    // try to match newline:
    unsigned char number_of_newline_chars = is_newline(data, data_length);
    if(number_of_newline_chars != 0){
        result.type = NEWLINE;
        result.number_of_characters = number_of_newline_chars;
        return result;
    }

    // try to match current address:
    /** current address is '$' without an identifier immediately following it */
    if(data[0] == '$' && (data_length == 1 || !valid_identifier_char_first(data[1]))){ 
        result.type = CURRENT_ADDRESS;
        result.number_of_characters = 1;
        return result;
    }

    // try to match 4 characters:
    if(data_length >= 4){
        if(starts_with(data, "WORD", data_length)) result.type = POINTER_WORD;
        else if(starts_with(data, "BYTE", data_length)) result.type = POINTER_BYTE;
        if(result.type != UNDETERMINED){
            result.number_of_characters = 5;
            return result;
        }
    }

    // try to match comments, variable length:
    if(data_length >= 2){
        if(data[0] == '/' && data[1] == '/'){
            result.type = COMMENT_SINGLE_LINE;
            result.number_of_characters = 2;
            while(1){
                if(is_newline(data, data_length) || data_length == 0) break;
                result.number_of_characters++;
                data++;
                data_length--;
            }
            return result;
        }else if(data[0] == '/' && data[1] == '*'){
            result.type = COMMENT_MULTI_LINE_INVALID;
            result.number_of_characters = 2;
            while(1){
                if(data_length >= 2 && data[0] == '*' && data[1] == '/'){
                    result.type = COMMENT_MULTI_LINE;
                    result.number_of_characters += 2;
                    break;
                }else if(data_length < 2){
                    break;
                }
                result.number_of_characters++;
                data++;
                data_length--;
            }
            return result;
        }
    }


    // still undetermined:
    return result;
}

token get_token(const char *data, unsigned int data_length){
    token result = get_simple_token(data, data_length);
    if(result.type != UNDETERMINED) return result;
    // try to match label:
    // at this point, '$' is definitely not current address, but label.
    if(data[0] == '$'){
        unsigned char identifier_length = get_identifier_length(&data[1], data_length - 1);
        if(identifier_length != INVALID_IDENTIFIER){
            result.type = LABEL;
            result.number_of_characters = identifier_length + 1;
            return result;
        }
    }
    unsigned char index;
    // try to match directive:
    index = search_array(data, data_length, strings_directives, strings_directives_length);
    if(index != NO_MATCH){
        result.type = DIRECTIVE;
        result.index = index;
        result.number_of_characters = strlen(strings_directives[result.index]);
        return result;
    }
    // try to match byte registers:
    index = search_array(data, data_length, strings_registers_byte, strings_registers_byte_length);
    if(index != NO_MATCH){
        result.type = REGISTER_BYTE;
        result.index = index;
        result.number_of_characters = strlen(strings_registers_byte[result.index]);
        return result;
    }
    // try to match word registers:
    index = search_array(data, data_length, strings_registers_word, strings_registers_word_length);
    if(index != NO_MATCH){
        result.type = REGISTER_WORD;
        result.index = index;
        result.number_of_characters = strlen(strings_registers_word[result.index]);
        return result;
    }
    // try to match segment registers:
    index = search_array(data, data_length, strings_registers_segment, strings_registers_segment_length);
    if(index != NO_MATCH){
        result.type = REGISTER_SEGMENT;
        result.index = index;
        result.number_of_characters = strlen(strings_registers_segment[result.index]);
        return result;
    }
    // try to match mnemonic:
    index = search_mnemonic_array(data, data_length);
    if(index != NO_MATCH){
        result.type = MNEMONIC;
        result.index = index;
        result.number_of_characters = strlen(instructions[index].mnemonic);
        return result;
    }
    // try to match integer or float constant:
    if(valid_digit_decimal(data[0])){
        if(data[0] == '0' && data_length >= 3){ // non decimal base
            result.number_of_characters = 2;
            if(lower_case(data[1]) == 'x'){
                result.type = CONSTANT_INTEGER_HEX;
                data += 2;
                data_length -= 2;
                while(data_length > 0 && (valid_digit_hex(data[0]) || data[0] == '_')){
                    data++;
                    data_length--;
                    result.number_of_characters++;
                }
                return result;
            }else if(lower_case(data[1]) == 'b'){
                result.type = CONSTANT_INTEGER_BINARY;
                data += 2;
                data_length -= 2;
                while(data_length > 0 && (valid_digit_binary(data[0]) || data[0] == '_')){
                    data++;
                    data_length--;
                    result.number_of_characters++;
                }
                return result;
            }else if(lower_case(data[1]) == 'o'){
                result.type = CONSTANT_INTEGER_OCTAL;
                data += 2;
                data_length -= 2;
                while(data_length > 0 && (valid_digit_octal(data[0]) || data[0] == '_')){
                    data++;
                    data_length--;
                    result.number_of_characters++;
                }
                return result;
            }
        }
        // decimal or floating point:
        result.type = CONSTANT_INTEGER_DECIMAL;
        result.number_of_characters = 0;
        while(data_length > 0){
            if(result.type == CONSTANT_INTEGER_DECIMAL && !((valid_digit_decimal(data[0]) || data[0] == '_'))){
                if(valid_digit_float(data[0])) result.type = CONSTANT_FLOAT;
                else break;
            }
            else if(result.type == CONSTANT_FLOAT && !((valid_digit_float(data[0]) || data[0] == '_'))) break;
            data++;
            data_length--;
            result.number_of_characters++;
        }
        return result;
    }
    // try to match char constant:
    if(data_length > 2 &&  data[0] == '\'' && data[2] == '\''){
        result.type = CONSTANT_CHAR;
        result.number_of_characters = 3;
        result.index = data[1];
        return result;
    }
    // try to match string constant:
    if(data[0] == '"'){
        result.type = CONSTANT_STRING_INVALID;
        result.number_of_characters = 1;
        while(data_length > 1){
            result.number_of_characters++;
            data_length--;
            data++;
            if(data[0] == '"'){
                result.type = CONSTANT_STRING;
                break;
            }
        }
        return result;
    }

    // return as other:
    result.type = OTHER;
    result.number_of_characters = 0;
    do{ // up to next simple token found (' ', '+','[')
        data++;
        data_length--;
        result.number_of_characters++;
    }while(get_simple_token(data, data_length).type == UNDETERMINED);
    return result;
}





/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////




const char* strings_token_types[] = {
    [COMMA] = ",",
    [COLON] = ":",
    [SEMICOLON] = ";",
    [NEWLINE] = "\\n",
    [BRACKET_SQUARE_LEFT] = "[",
    [BRACKET_SQUARE_RIGHT] = "]",
    [OPERATOR_PLUS] = "+",
    [OPERATOR_MINOS] = "-",
    [CURRENT_ADDRESS] = "$",

    [POINTER_WORD] = "word",
    [POINTER_BYTE] = "byte",

    [SPACE] = " ",
    [COMMENT_SINGLE_LINE] = "//",
    [COMMENT_MULTI_LINE] = "/* */",
    [COMMENT_MULTI_LINE_INVALID] = "/*INVALID*/",
    [CONSTANT_INTEGER_DECIMAL] = "decimal",
    [CONSTANT_INTEGER_BINARY] = "binary",
    [CONSTANT_INTEGER_HEX] = "hex",
    [CONSTANT_INTEGER_OCTAL] = "octal",
    [CONSTANT_FLOAT] = "float",
    [CONSTANT_CHAR] = "char",
    [CONSTANT_STRING] = "string",
    [CONSTANT_STRING_INVALID] = "INVALID string",
    [LABEL] = "label",
    [DIRECTIVE] = "directive",
    [REGISTER_BYTE] = "register byte",
    [REGISTER_WORD] = "register word",
    [REGISTER_SEGMENT] = "register segment",
    [MNEMONIC] = "mnemonic",
    
    
    // other:
    [END_OF_BUFFER] = "end of buffer",
    [OTHER] = "other",
    [UNDETERMINED] = "undetermined"
};

void print_token(token t){
    if(t.type == MNEMONIC) printf("'%s'", instructions[t.index].mnemonic);
    else if(t.type == DIRECTIVE) printf("%s", strings_directives[t.index]);
    else if(t.type == REGISTER_BYTE) printf("%s", strings_registers_byte[t.index]);
    else if(t.type == REGISTER_WORD) printf("%s", strings_registers_word[t.index]);
    else if(t.type == REGISTER_SEGMENT) printf("%s", strings_registers_segment[t.index]);
    else if(t.type == OTHER) printf("OTHER");
    else printf("'%s'", strings_token_types[t.type]);
    
}