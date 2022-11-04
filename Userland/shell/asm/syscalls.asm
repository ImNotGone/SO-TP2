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
    syscallkill     equ 14
    syscallwaitpid  equ 15
    syscallnice     equ 16
    syscallblock    equ 17
    syscallunblock  equ 18
    syscallyield    equ 19
    syscallgetpid   equ 20
    
    syscallsemopen    equ 21
    syscallsemwait    equ 22
    syscallsempost    equ 23
    syscallsemclose   equ 24
    syscallsemunlink  equ 25
    syscallseminfo    equ 26
    syscallseminit    equ 27
    syscallsemdestroy equ 28


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
global syskill
global syswaitpid
global sysnice
global sysblock
global sysunblock
global sysyield
global sysgetpid

global syssemopen
global syssemwait
global syssempost
global syssemclose
global sysseminfo
global sysseminit
global syssemdestroy

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
; rsi -> priority
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

;rdi -> pid
syskill:
    syscallHandler syscallkill

;rdi -> pid
syswaitpid:
    syscallHandler syscallwaitpid

;rdi->pid
;rsi->priority
sysnice:
    syscallHandler syscallnice

;rdi->pid
sysblock:
    syscallHandler syscallblock

;rdi->pid
sysunblock:
    syscallHandler syscallunblock

sysyield:
    syscallHandler syscallyield

sysgetpid:
    syscallHandler syscallgetpid

;rdi->name
;rsi->value
syssemopen:
    syscallHandler syscallsemopen

;rdi->sem
syssemwait:
    syscallHandler syscallsemwait

;rdi->sem
syssempost:
    syscallHandler syscallsempost

;rdi->sem
syssemclose:
    syscallHandler syscallsemclose

;rdi->name
syssemunlink:
    syscallHandler syscallsemunlink

;rdi->size
sysseminfo:
    syscallHandler syscallseminfo

;rdi->value
sysseminit:
    syscallHandler syscallseminit

;rdi->sem
syssemdestroy:
    syscallHandler syscallsemdestroy
