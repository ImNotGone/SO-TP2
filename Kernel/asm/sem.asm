GLOBAL _xadd
GLOBAL _xchg

SECTION .text

; _xadd(inc, &value);
_xadd:
  mov rax, rdi
  lock xadd [rsi], eax
  ret

; _xchg(&lock, value);
_xchg:
  mov rax, rsi
  xchg [rdi], eax
  ret
