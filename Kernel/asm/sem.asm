GLOBAL _xadd
GLOBAL _xchg

SECTION .text

_xadd:
  mov rax, rdi
  lock xadd [rsi], eax
  ret

_xchg:
  mov rax, rsi
  xchg [rdi], eax
  ret
