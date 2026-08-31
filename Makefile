CC = gcc

SRC_DIR = src
INC_DIR = include
OUT_DIR = out

INCLUDES = $(addprefix -I, $(INC_DIR))

CFLAGS = -O2 $(INCLUDES)

UTIL_HEADER = $(INC_DIR)/util.h
UTIL_SOURCE = $(SRC_DIR)/util.c
UTIL_OBJECT	= $(OUT_DIR)/util.o

MAIN_SRCS = $(filter-out $(UTIL_SOURCE), $(wildcard $(SRC_DIR)/*.c))

EXECUTABLES = $(patsubst $(SRC_DIR)/%.c, $(OUT_DIR)/%, $(MAIN_SRCS))

.PHONY: all clean

all: $(EXECUTABLES)

$(UTIL_OBJECT) : $(UTIL_SOURCE) $(UTIL_HEADER)
	@mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT_DIR)/%: $(SRC_DIR)/%.c $(UTIL_OBJECT)
	@mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) -o $@ $< $(UTIL_OBJECT)

beautify:
	rm -rf $(OUT_DIR)/*.o
clean:
	rm -rf $(OUT_DIR) $(SRC_DIR)/*.o