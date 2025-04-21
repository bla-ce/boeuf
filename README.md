# boeuf - Dynamic Buffer in Assembly

**boeuf** is a library used to create and manage dynamic buffers in Netwide Assembly. Dynamic buffers are referred as `boeuf` in this library.

A complete usage example can be found in `/example`

## Usage

**Create a boeuf buffer**

```assembly
mov     rdi, buf        ; pointer to the string
call    boeuf_create
cmp     rax, 0
jl      .error
```

**Append string to a boeuf buffer**

```assembly
mov     rdi, [rsp]      ; pointer to the boeuf
call    boeuf_append
cmp     rax, 0
jl      .error
```

**Reset boeuf buffer**

```assembly
mov     rdi, [rsp]      ; pointer to the boeuf
call    boeuf_reset
cmp     rax, 0
jl      .error
```

**Get length of boeuf and set a new (smaller one)**

```assembly
mov     rdi, [rsp]      ; pointer to the boeuf
call    boeuf_len
cmp     rax, 0
jl      .error

mov     rdi, [rsp]      
mov     rsi, 5          ; assume that the length is smaller
call    boeuf_set_len
cmp     rax, 0
jl      .error
```

**Print error messages**

```assembly
mov     rdi, 10         ; rdi should get a pointer to the boeuf, it will fail
mov     rsi, buf
call    boeuf_append
cmp     rax, 0
jl      .error

; ... logic

.error:
call    boeuf_perror    ; will print '[ERROR] error_message'
``` 

**Free the boeuf**

```assembly
mov     rdi, [rsp]      ; pointer to the boeuf
call    boeuf_free
cmp     rax, 0
jl      .error
```

