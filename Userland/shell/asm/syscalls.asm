section .data
    syscallread     equ 0
    syscallwrite    equ 1

    syscallmalloc   equ 2
    syscallfree     equ 3
    syscallrealloc  equ 4
    syscallcalloc   equ 5
    syscallmeminfo  equ 6

    syscallcreateprocess equ 7
    syscallexit     equ 8
    syscallps       equ 10
    syscallkill     equ 11
    syscallwaitpid  equ 12
    syscallnice     equ 13
    syscallblock    equ 14
    syscallunblock  equ 15
    syscallsleep    equ 16
    syscallyield    equ 17
    syscallgetpid   equ 18

    syscallsemopen    equ 19
    syscallsemwait    equ 20
    syscallsempost    equ 21
    syscallsemclose   equ 22
    syscallsemunlink  equ 23
    syscallseminfo    equ 24
    syscallseminit    equ 25
    syscallsemdestroy equ 26

    syscalldup        equ 27
    syscallpipe       equ 28
    syscallclose      equ 29
    syscallmkfifo     equ 30
    syscallunlink     equ 31
    syscallpipeinfo   equ 32


section .text
global sysread
global syswrite

global sysmalloc
global sysfree
global sysrealloc
global syscalloc
global sysmeminfo

global syscreateprocess
global sysexits
global sysps
global syskill
global syswaitpid
global sysnice
global sysblock
global sysunblock
global syssleep
global sysyield
global sysgetpid

global syssemopen
global syssemwait
global syssempost
global syssemclose
global syssemunlink
global sysseminfo
global sysseminit
global syssemdestroy

global sysdup
global syspipe
global sysclose
global sysmkfifo
global sysunlink
global syspipeinfo

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

;rdi->pid
;rsi->seconds
syssleep:
    syscallHandler syscallsleep

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

;rdi->pid
;rsi->prev fd
;rdx->new fd
sysdup:
    syscallHandler syscalldup

; rdi-> fd[2]
syspipe:
    syscallHandler syscallpipe

;rdi->fd
sysclose:
    syscallHandler syscallclose

;rdi->name
sysmkfifo:
    syscallHandler syscallmkfifo

;rdi->name
sysunlink:
    syscallHandler syscallunlink

;rdi->size
syspipeinfo:
    syscallHandler syscallpipeinfo
