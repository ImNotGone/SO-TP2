section .text

global createProcess

;call r9 due to the fact that entryWrapper is larger than a register.
; what happens with de last registers, is this a problem?
; maybe removing the entrypoint could solve this
entryWrapper:
    call r9
    mov rax, 0x0B
    int 80h

; creates a process' context
; createProcess(int stackBase, int rip, int argc, char* argv[])
createProcess:
    push rbp
    mov rbp, rsp

    mov rsp, rdi
    push 0x0
    push rdi
    push 0x202
    push 0x8
    push entryWrapper

    push rax
    push rbx
    push rcx
    push rdx
    push rbp
    push rdi ; aca va rdx capaz
    push rsi ; aca rcx capaz
    push r8
    push r9
    push r10
	push r11
	push r12
	push r13
	push r14
	push r15

    mov rax, rsp
    mov rsp, rbp
    pop rbp
    ret


