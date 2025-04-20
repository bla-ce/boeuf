# boeuf - Dynamic Buffer in Assembly

**boeuf** is a library written in x86 Assembly. It provides simple methods to:

- **Initialize** a resizable buffer  
- **Append** NULL‑terminated strings  
- **Reset** the buffer
- **Set** new (smaller) length of the buffer
- **Get** length of the buffer
- **Free** the buffer when done  

boeuf delegates all memory management to the [unstack](https://github.com/bla-ce/unstack) library.

## Features

- **Dynamic growth**: Buffers automatically expand as you append data.  
- **Easy string handling**: Append strings to the buffer.  
- **Example included**: See the `/example` directory for a complete usage demonstration.

