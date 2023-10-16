#ifndef PRINT_H
#define PRINT_H

#include "stdio.h"

// ensuring macros passed in are expanded before stringizing
#define _TO_STRING(x)   #x
#define TO_STRING(x)    _TO_STRING(x)

#define COLOR_RED               "\033[91m"
#define COLOR_GREEN             "\033[92m"
#define COLOR_YELLOW            "\033[93m"
#define COLOR_BLUE              "\033[94m"
#define COLOR_MAGENTA           "\033[95m"
#define COLOR_CYAN              "\033[96m"
#define COLOR_END               "\033[0m"

#if 1
    #define log(...)  printf(COLOR_MAGENTA __FILE__":" TO_STRING(__LINE__)": " COLOR_END  __VA_ARGS__)
    #define warning(...)  printf(COLOR_YELLOW __FILE__":" TO_STRING(__LINE__)": warning: " COLOR_END  __VA_ARGS__)
    #define error(...)  fprintf(stderr, COLOR_RED __FILE__":" TO_STRING(__LINE__)": error: " COLOR_END  __VA_ARGS__)
    #define print(...)  printf( __VA_ARGS__)
#else
    #define log(...)        (void)0
    #define warning(...)    (void)0
    #define error(...)      (void)0
    #define print(...)      (void)0
#endif

#endif /* PRINT_H */