%include "string.inc"
%include "malloc.inc"
%include "realloc.inc"
%include "free.inc"
%include "mmap.inc"
%include "constants.inc"
%include "ascii.inc"
%include "syscall.inc"
%include "file.inc"

; TODO: add magic value checks

section .data
  boeuf_metadata_struct:
    boeuf_magic_value dq 0xC0FFEE
    boeuf_size        dq 0
    boeuf_length      dq 0
  boeuf_metadata_struct_end:

  BOEUF_METADATA_LEN equ boeuf_metadata_struct_end - boeuf_metadata_struct

  ; offsets  
  BOEUF_METADATA_OFF_MAGIC_VALUE  equ 0
  BOEUF_METADATA_OFF_SIZE         equ BOEUF_METADATA_OFF_MAGIC_VALUE + 0x8
  BOEUF_METADATA_OFF_LENGTH       equ BOEUF_METADATA_OFF_SIZE + 0x8
  BOEUF_METADATA_OFF_DATA         equ BOEUF_METADATA_OFF_LENGTH + 0x8

  MAX_BOEUF_BUF_SIZE equ 0x800

section .text
; creates a growing buffer 
; @param  rdi: pointer to the string
; @return rax: pointer to the buffer
boeuf_create:
  sub   rsp, 0x18

  cmp   rdi, 0
  jl    .error

  ; *** STACK USAGE *** ;
  ; [rsp]       -> pointer to the string
  ; [rsp+0x8]   -> length of the string
  ; [rsp+0x10]  -> pointer to the boeuf buffer 

  mov   [rsp], rdi

  ; get size of the string
  call  strlen
  cmp   rax, 0
  jl    .error

  mov   qword [rsp+0x8], rax

  ; add metadata struct length to store the size of the buffer
  mov   rdi, rax
  add   rdi, BOEUF_METADATA_LEN
  inc   rdi   ; add one for null_char
  call  malloc
  cmp   rax, 0
  jl    .error

  ; init metadata
  mov   rbx, qword [boeuf_magic_value]
  mov   qword [rax+BOEUF_METADATA_OFF_MAGIC_VALUE], rbx

  mov   rbx, qword [rsp+0x8]
  mov   qword [rax+BOEUF_METADATA_OFF_SIZE], rbx

  mov   qword [rax+BOEUF_METADATA_OFF_LENGTH], rbx

  ; return pointer to the data
  add   rax, BOEUF_METADATA_OFF_DATA

  mov   [rsp+0x10], rax

  ; copy string to the boeuf buffer
  mov   rdi, rax
  mov   rsi, [rsp]
  mov   rcx, qword [rsp+0x8]
  rep   movsb

  ; add null char
  mov   rax, NULL_CHAR
  stosb

  mov   rax, [rsp+0x10]

  jmp   .return

.error:
  mov   rax, FAILURE_CODE

.return:
  add   rsp, 0x18
  ret

; appends string to a buffer
; @param  rdi: pointer to the boeuf buffer
; @param  rsi: pointer to the string to be appended
; @return rax: return code
boeuf_append:
  sub   rsp, 0x30

  cmp   rdi, 0
  jl    .error

  cmp   rsi, 0
  jl    .error

  ; *** STACK USAGE *** ;
  ; [rsp]       -> pointer to the boeuf buffer
  ; [rsp+0x8]   -> pointer to the string
  ; [rsp+0x10]  -> pointer to boeuf metadata
  ; [rsp+0x18]  -> length of the string
  ; [rsp+0x20]  -> size of new boeuf buffer if reallocated
  ; [rsp+0x28]  -> pointer to new boeuf buffer if reallocated

  mov   [rsp], rdi
  mov   [rsp+0x8], rsi

  ; make sure of the integrity of the boeuf buffer
  sub   rdi, BOEUF_METADATA_LEN 
  mov   rbx, qword [boeuf_magic_value]
  cmp   qword [rdi+BOEUF_METADATA_OFF_MAGIC_VALUE], rbx
  jne   .error

  ; get length of string to be appended
  mov   rdi, [rsp+0x8]
  call  strlen
  cmp   rax, 0
  jl    .error

  mov   qword [rsp+0x18], rax
  
  ; get pointer to the boeuf struct
  mov   rdi, [rsp]
  sub   rdi, BOEUF_METADATA_LEN
  mov   [rsp+0x10], rdi

  ; check if boeuf needs to grow
  mov   rax, qword [rdi+BOEUF_METADATA_OFF_LENGTH]
  mov   rbx, qword [rdi+BOEUF_METADATA_OFF_SIZE]
  mov   rcx, qword [rsp+0x18]

  ; add current length of the boeuf buffer and the length of the string to be appended
  add   rax, rcx
  
  ; compare it with the size of the boeuf buffer
  cmp   rax, rbx
  jl    .no_grow

  ; save new size onto the stack
  mov   qword [rsp+0x20], rax

  ; malloc new boeuf buffer
  mov   rax, qword [rdi+BOEUF_METADATA_OFF_SIZE]
  add   rax, qword [rsp+0x18]
  add   rax, BOEUF_METADATA_LEN
  
  ; make sure the requested size is not over MAX_BOEUF_BUF_SIZE
  cmp   rax, MAX_BOEUF_BUF_SIZE
  jg    .error

  ; realloc chunk of memory
  mov   rdi, [rsp+0x10]
  mov   rsi, rax
  inc   rsi       ; add one for null char
  call  realloc
  cmp   rax, 0
  jl    .error

  mov   [rsp+0x28], rax

  ; populate metadata of new boeuf buffer
  mov   rbx, qword [boeuf_magic_value]
  mov   qword [rax+BOEUF_METADATA_OFF_MAGIC_VALUE], rbx

  ; add size of the boeuf buffer
  mov   rbx, qword [rsp+0x20]
  mov   qword [rax+BOEUF_METADATA_OFF_SIZE], rbx

  ; compute new length of the buffer
  mov   rdi, [rsp+0x10]

  ; get current length of old buffer
  mov   rbx, [rdi+BOEUF_METADATA_OFF_LENGTH]

  ; add length of string to append
  add   rbx, qword [rsp+0x18]
  mov   qword [rax+BOEUF_METADATA_OFF_LENGTH], rbx

  ; go after buffer to append second string
  add   rax, BOEUF_METADATA_OFF_DATA
  add   rax, [rdi+BOEUF_METADATA_OFF_LENGTH]

  ; move second string
  mov   rdi, rax
  mov   rsi, [rsp+0x8]
  mov   rcx, [rsp+0x18]
  rep   movsb

  ; add null_char
  mov   rax, NULL_CHAR
  stosb

  ; free old boeuf buffer
  mov   rdi, [rsp+0x10]
  call  free
  cmp   rax, 0
  jl    .error

  mov   rax, [rsp+0x28]
  add   rax, BOEUF_METADATA_OFF_DATA

  jmp   .return

.no_grow:
  mov   rdi, [rsp+0x10]
  mov   rax, qword [rdi+BOEUF_METADATA_OFF_LENGTH]

  mov   rcx, qword [rsp+0x18]
  add   qword [rdi+BOEUF_METADATA_OFF_LENGTH], rcx

  mov   rdi, [rsp]
  add   rdi, rax

  mov   rsi, [rsp+0x8]
  rep   movsb

  ; add null char
  mov   rax, NULL_CHAR
  stosb
  
  mov   rax, [rsp]
  jmp   .return

.error:
  mov   rax, FAILURE_CODE

.return:
  add   rsp, 0x30
  ret

; frees the boeuf buffer
; @param  rdi: pointer to the boeuf buffer to freed
; @return rax: return code
boeuf_free:
  sub   rdi, BOEUF_METADATA_LEN

  ; make sure of the integrity of the boeuf buffer
  mov   rbx, qword [boeuf_magic_value]
  cmp   qword [rdi+BOEUF_METADATA_OFF_MAGIC_VALUE], rbx
  jne   .error

  call  free

  jmp   .return

.error:
  mov   rax, FAILURE_CODE

.return:
  ret

; resets the boeuf buffer
; @param  rdi: pointer to the boeuf buffer
; @return rax: return code
boeuf_reset:
  cmp   rdi, 0
  jl    .error

  ; add null char at the beginning of the string
  mov   rax, NULL_CHAR
  stosb

  dec   rdi
  sub   rdi, BOEUF_METADATA_LEN 

  ; make sure of the integrity of the boeuf buffer
  mov   rbx, qword [boeuf_magic_value]
  cmp   qword [rdi+BOEUF_METADATA_OFF_MAGIC_VALUE], rbx
  jne   .error

  ; set length to 0
  mov   qword [rdi+BOEUF_METADATA_OFF_LENGTH], 0

  mov   rax, SUCCESS_CODE
  jmp   .return

.error:
  mov   rax, FAILURE_CODE

.return:
  ret

; returns the length of the boeuf buffer
; @param  rdi: pointer to the boeuf buffer
; @return rax: length of the boeuf buffer
boeuf_len:
  cmp   rdi, 0
  jl    .error

  sub   rdi, BOEUF_METADATA_LEN

  ; make sure of the integrity of the boeuf buffer
  mov   rbx, qword [boeuf_magic_value]
  cmp   qword [rdi+BOEUF_METADATA_OFF_MAGIC_VALUE], rbx
  jne   .error

  mov   rax, qword [rdi+BOEUF_METADATA_OFF_LENGTH]

  jmp   .return

.error:
  mov   rax, FAILURE_CODE

.return:
  ret

; sets the new (smaller) length of the boeuf buffer
; @param  rdi: pointer to the boeuf buffer
; @param  rsi: new size
; @return rax: return code
set_boeuf_len:
  sub   rsp, 0x10

  cmp   rdi, 0
  jl    .error

  cmp   rsi, 0
  jl    .error

  ; *** STACK USAGE *** ;
  ; [rsp]     -> pointer to the boeuf buffer
  ; [rsp+0x8] -> new size of the boeuf buffer

  mov   [rsp], rdi
  mov   [rsp+0x8], rsi

  sub   rdi, BOEUF_METADATA_LEN

  ; make sure of the integrity of the boeuf buffer
  mov   rbx, qword [boeuf_magic_value]
  cmp   qword [rdi+BOEUF_METADATA_OFF_MAGIC_VALUE], rbx
  jne   .error

  cmp   rsi, qword [rdi+BOEUF_METADATA_OFF_LENGTH]
  jge   .return

  mov   qword [rdi+BOEUF_METADATA_OFF_LENGTH], rsi
  add   rdi, BOEUF_METADATA_LEN
  add   rdi, rsi

  mov   rax, NULL_CHAR
  stosb

  mov   rax, SUCCESS_CODE
  jmp   .return

.error:
  mov   rax, FAILURE_CODE

.return:
  add   rsp, 0x10
  ret

