section .text
global createProcess

entryWrapper:
    call rdx
    mov rax, 0x08 ; sysexit syscall
    int 80h

; creates a process' context
; createProcess(int stackBase, int rip, int argc, char* argv[])
createProcess:
    push rbp
    mov rbp, rsp

    mov rsp, rdi ; stack base
    and rsp, -16
    push 0x0
    push rdi
    push 0x202
    push 0x8
    push entryWrapper

   ;obs: rdi rsi , rdx and rcx have been swapped
    push r15
	push r14
	push r13
	push r12
	push r11
	push r10
	push r9
	push r8

    ;for processes arguments
	push rcx ; argv -> rsi
	push rdx ; argc -> rdi

	push rbp
	push rsi ; rip (rdx)
	push rdi ; stackbase (not necessary) (rcx)
	push rbx
	push rax

    mov rax, rsp
    mov rsp, rbp
    pop rbp
    ret


