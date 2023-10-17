// general assembly

mov SP, 0x0
mov ax, [ES:BP + 0x10]
jmp 0xf000:0xc000
HLT
mov byte [14], 0
out 0, al
add sp,4
mov bx, 0x1234
push bx
pop ax; pop ax ;pop ax
inc ax
mov byte [es: 0000], 0
mov al, [SS : bx + si]
mov [SS : 0x950 + si], al
cli; STI
mov cx, 5
shl cl, 1
REP MOVSB // moves cx bytes from ds:si to es:di
