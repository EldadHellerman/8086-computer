#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <inttypes.h>

// Preprocessor directives:
extern const char *strings_preprocessor_directives[];
extern uint8_t strings_preprocessor_directives_length;

void preprocessor(void);

#endif /* PREPROCESSOR_H */