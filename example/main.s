global _start

%include "boeuf.inc"

section .data
  buf   db "Hello", NULL_CHAR
  buf2  db ", World! ", NULL_CHAR
  buf3  db "I am a buffer growing. ", NULL_CHAR
  buf4  db "And I can grow indefinitely. ", NULL_CHAR
  buf5  db "As long as the program as enough memory. ", NULL_CHAR
  buf6  db "How cool is that? I find that cool", NULL_CHAR

section .bss

section .text
_start:
  sub   rsp, 0x8

  mov   rdi, 8
  call  boeuf_create
  cmp   rax, 0
  jl    .error

  mov   [rsp], rax 

  mov   rdi, rax
  mov   rsi, buf
  call  boeuf_append
  cmp   rax, 0
  jl    .error

  mov   [rsp], rax

  mov   rdi, [rsp]
  call  println

  mov   rdi, [rsp]
  mov   rsi, buf2
  call  boeuf_append
  cmp   rax, 0
  jl    .error

  mov   [rsp], rax

  mov   rdi, [rsp]
  call  println

  mov   rdi, [rsp]
  mov   rsi, buf3
  call  boeuf_append
  cmp   rax, 0
  jl    .error

  mov   [rsp], rax

  mov   rdi, [rsp]
  call  println

  mov   rdi, [rsp]
  mov   rsi, buf4
  call  boeuf_append
  cmp   rax, 0
  jl    .error

  mov   [rsp], rax

  mov   rdi, [rsp]
  call  println

  mov   rdi, [rsp]
  mov   rsi, buf5
  call  boeuf_append
  cmp   rax, 0
  jl    .error

  mov   [rsp], rax

  mov   rdi, [rsp]
  call  println

  mov   rdi, [rsp]
  mov   rsi, buf6
  call  boeuf_append
  cmp   rax, 0
  jl    .error

  mov   [rsp], rax

  mov   rdi, rax
  call  println

  ; free boeuf buffer
  mov   rdi, [rsp]
  call  boeuf_free

  mov   rdi, SUCCESS_CODE
  call  exit

.error:
  mov   rdi, FAILURE_CODE
  call  exit

; exits the program with return code
exit:
  mov   rax, SYS_EXIT
  syscall

