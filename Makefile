INC_DIR = inc
UTILS_DIR = utils
MALLOC_DIR = malloc
INCLUDES = $(shell find $(INC_DIR) -type f -name '*.inc')

main: $(INCLUDES) main.s
	nasm -f elf64 -o main.o main.s \
		-g -w+all  \
		-I$(INC_DIR)/ \
		-I$(INC_DIR)/$(UTILS_DIR) \
		-I$(INC_DIR)/$(MALLOC_DIR)
	ld -o main main.o

.PHONY: clean
clean:
	rm -f main main.o

