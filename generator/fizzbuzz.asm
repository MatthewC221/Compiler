;nasm -f elf32 register_print.asm
;gcc -m32 register_print.o -o register_print

[SECTION .data]
PrintNum    db "%d",10,0 ;this is a c string so is null terminated
[SECTION .text]
extern printf       
global main

main:
    
    mov ecx, 10

print:
    push ebp
    mov ebp,esp
    push ebx
    push esi
    push edi        ; stuff before this for glibc compatibility

    mov eax, ecx
    push eax        ; push eax onto stack then the format string, then call printf to write eax to console, unwind stack pointer
    push PrintNum
    call printf 
    add esp,8   

    pop edi         ; stuff after this for glibc compatibility
    pop esi
    pop ebx
    mov esp,ebp
    pop ebp
    ret
    
next:
    jmp print
    jmp print
    jmp print
    
    ret
    

