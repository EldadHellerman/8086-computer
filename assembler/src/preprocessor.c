#include "preprocessor.h"
#include "print.h"
#include "stdlib.h"

// Preprocessor directives:

const char *strings_preprocessor_directives[] = {
    "#include",
    "#define",
    "#if",
    "#ifndef",
    "#endif",
    "#macro",
    "#endmacro",
    "__FILE__",
    "__LINE__"
};

uint8_t strings_preprocessor_directives_length = sizeof(strings_preprocessor_directives) / sizeof(char*);



void preprocessor(void){
    print("hello!\n");
}