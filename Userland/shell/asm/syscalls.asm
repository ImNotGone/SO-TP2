section .data
    syscallread     equ 0
    syscallwrite    equ 1
    syscalltime     equ 2
    syscallmemdump  equ 3
    syscallregdump  equ 4
    syscallmalloc   equ 5
    syscallfree     equ 6
    syscallrealloc  equ 7
    syscallcalloc   equ 8
    syscallmeminfo  equ 9
    syscallcreateprocess equ 10
    syscallexit     equ 11
    syscallexec     equ 12
    syscallps       equ 13


section .text
global sysread
global syswrite
global systime
global sysmemdump
global sysregdump
global sysmalloc
global sysfree
global sysrealloc
global syscalloc
global sysmeminfo
global syscreateprocess
global sysexits
global sysexec
global sysps

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

; rdi -> nmemb
; rsi -> size
syscalloc:
    syscallHandler syscallcalloc

; rdi -> void *
sysmeminfo:
    syscallHandler syscallmeminfo

; rdi -> void *
; rsi -> ground
; priority
; rdx -> argc
; rcx -> argv[]
syscreateprocess:
    syscallHandler syscallcreateprocess

sysexits:
    syscallHandler syscallexit

;rdi -> pid
sysexec:
    syscallHandler syscallexec

sysps:
    syscallHandler syscallps
