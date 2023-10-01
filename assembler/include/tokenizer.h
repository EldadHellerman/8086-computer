#ifndef TOKENIZER_H
#define TOKENIZER_H
#include "isa.h"

typedef enum{
    MNEMONIC,
    REGISTER_BYTE,
    REGISTER_WORD,
    REGISTER_SEGMENT,
    POINTER_WORD,
    POINTER_BYTE,
    SEGMENT_OVERRIDE,
    DIRECTIVE,
    COLON,
    NUMBER,
    PLUS,
    MINOS,
    DIVIDE,
    NEWLINE,

} token_type;

typedef struct{
    token_type type;
    union{
        struct{
            int mnemonic_formats_length;
            if_t mnemonic_formats[];
        };
        int register_number;

    };
} token;

token get_next_token(void);

#endif /* TOKENIZER_H */