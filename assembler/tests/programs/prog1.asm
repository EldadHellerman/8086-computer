// general assembly

.global $label_global
.extern $main

.section .reset
    $_reset:
        mov SP, 0x0
        jmp 0x0000:$_start

.section .IVT
$interrupt_table_start:
.times 256 .db 0x00, 0x04, 0x00, 0x00
.dw $interrupt_table_start + 0x1234,0x0000
.db 0x00, 'c', "bye", 0x00, 0b1001_0011, 0O712, 1.34, 10e-56, 'h'

.section .text
$_start:
    mov ax, [ES:BP + 0x10]
    jmp 0xf000:0xc000
    call $main
    $end_loop:
        HLT
        jmp $end_loop

$main:
    mov byte [14], 0
    out 0, al
    add sp,4
    mov bx, 0x1234
    push bx
    push 0x1234
    push 0x12

    pop ax
    pop ax
    pop ax
    inc ax
    mov byte [es: 0000], 0
    mov al, [SS : bx + si]
    mov [SS : 0x950 + si], al

    delay_ms 100
    draw_string $str_welcome_2, 0, 20
    register_interrupt 0, $interrupt_0, 0
    cli; STI
    mov cx, 5
    shl cl, 1
    $counter:
        mov bx,cx
        mov byte [bx + 0x10], cl
    loop $counter
    cmp cx, 0
    je $is_zero
        mov byte [0x1a], 0x11    
        jmp $next
    $is_zero:
        mov byte [0x1b], 0x22
        jmp $next
    $next:
    
    call $function_add

    mov CX, $message_end - $message
    mov ax, 0 //seg message
    mov DS, ax
    mov SI, $message
    mov AX, 0 //seg memory_start
    mov ES, AX
    mov DI, $memory_start
    REP MOVSB //moves cx bytes from ds:si to es:di

$function_add:
    add ax, bx
    ret
$interrupt_0:
    IRET

$message:
.DB "eldad's 8086",'a', 'b' ,'A' , 'B','\0'
$message_end:

.times 0x1000 - $ .db 0x90 //fill memory until specified address

.equ    WHITE      0xFFFF
.equ    BLACK      0x0000

.section .files
$file_image_bin:
.incbin "image.bin"

