# Assembly syntax and directives

Comments:
```
// this is a one line comment
/* this is
a multiline comment */
```

labels:  
```
/*
All labels start with '$'.
'$' is the location at the start of current line.
*/

$jump: //defining label
    mov ax, $jump //using label
    mov ax, [$jump] //using label
    jmp $continue + 1 // label math, skip NOP
$continue:
    NOP
    JMP $ // infinite loop
```

Line endings:
```
/*
';' is used to seperate commands on the same line.
'\n' or '\r\n' are used to move to next line.
*/
mov ax, 5; mov bx, 6  ;mov cx,7  // Valid. 3 commands in one line.
```

Assembler directives:
All assembler directives start with a dot.
```
.incbin "file.bin"
.section .text
.extern main
```

Constants:
```
// constants prefixes are case insensitive

// integers (underscore seperators are allowed):
.db -24, +35_000                // decimal
.db 0x12_34                     // hex
.db 0o45                        // octal
.db 0b0110_0011                 // binary
.db 'a'                         // ascii
.dd 2.0, -2f, 1.23, +4.35e-2    // float

// String constants:
"str" // This is considered the same as 's', 't', 'r', '\0'.
```

General:
```
mov DST, SRC // DST is on the left, SRC is on the right.

mov byte [0x3456], 0x20
mov AL, byte [0x3456]
mov AL, [0x3456] // Implicit byte. Optional warning.

mov [0x3456], 0x20 // Unknowen size. Error.
inc [0x3456] // Unknowen size. Error.

mov ax, [ES : 0x04] // Segment override is inside the square brackets.
```

# Preprocessor Directives

```
#include "file"
#ifndef GUARD
#def GUARD
#endif

#define const 5

#define macro(x) mov ax, 5
#define macro2(a,b) (bp + si + a*8 + b)

#macro macro3
    mov ax, 0
    ret
#endmacro
```

Comments:
```
// this is a one line comment
/* this is
a multiline comment */
```
