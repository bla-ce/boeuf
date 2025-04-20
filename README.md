# boeuf - Dynamic Buffer in Assembly

**boeuf** is a library written in x86 Assembly. It provides simple methods to:

- **Initialize** a resizable buffer  
- **Append** NULL‑terminated strings  
- **Reset** the buffer
- **Free** the buffer when done  

boeuf delegates all memory management to the [unstack](https://github.com/bla-ce/unstack) library.

## Features

- **Dynamic growth**: Buffers automatically expand as you append data.  
- **Easy string handling**: Append strings to the buffer.  
- **Example included**: See the `/example` directory for a complete usage demonstration.

## Getting Started

1. Clone this repository.  
2. Assemble and link your code against `boeuf` and `unstack`.
3. Use `boeuf_create`, `boeuf_append`, `boeuf_reset` and `boeuf_free` in your Assembly programs.

