section .data
    syscallread     equ 0
    syscallwrite    equ 1
    syscalltime     equ 2
    syscallmemdump  equ 3
    syscallregdump  equ 4
    syscallmalloc   equ 5
    syscallfree     equ 6
    syscallrealloc  equ 7
    syscall meminfo equ 8


section .text
global sysread
global syswrite
global systime
global sysmemdump
global sysregdump
global sysmalloc
global sysfree

%macro syscallHandler 1
    push rbp
    mov rbp, rsp

    mov rax, %1
    int 80h

    mov rsp, rbp
    pop rbp
    ret
%endmacro

sysread:
    syscallHandler syscallread

;
; rdi -> fd
; rsi -> buff
; rdx -> len
;
syswrite:
    syscallHandler syscallwrite

; rdi -> Ttime *
systime:
    syscallHandler syscalltime

; rdi -> direccion
; rsi -> memData[]
sysmemdump:
    syscallHandler syscallmemdump

; rdi -> TRegs *
sysregdump:
    syscallHandler syscallregdump

; rdi -> size
sysmalloc:
    syscallHandler syscallmalloc

; rdi -> ptr *
sysfree:
    syscallHandler syscallfree

; rdi -> ptr *
; rsi -> size
sysrealloc:
    syscallHandler syscallrealloc

; rdi -> void *
sysmeminfo:
    syscallHandler syscallmeminfo
