INC_DIR = inc
UTILS_DIR = utils
MALLOC_DIR = malloc
INCLUDES = $(shell find $(INC_DIR) -type f -name '*.inc')

main: $(INCLUDES) example/main.s
	nasm -f elf64 -o example/main.o example/main.s \
		-g -w+all  \
		-I$(INC_DIR)/ \
		-I$(INC_DIR)/$(UTILS_DIR) \
		-I$(INC_DIR)/$(MALLOC_DIR)
	ld -o example/main example/main.o

.PHONY: clean
clean:
	rm -f example/main example/main.o

