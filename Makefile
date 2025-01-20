INC_DIR := include
SRC_DIR := src 
BIN_DIR := bin

OUTPUT := msh
EXE := $(BIN_DIR)/$(OUTPUT)
CC := gcc
CPPFLAGS := -I$(INC_DIR)
CFLAGS := -g -Wall -fsanitize=address -std=c99 -D_POSIX_C_SOURCE
LFLAGS := -lm

SRC := $(wildcard src/*.c)
INC := $(wildcard $(INC_DIR)/*.h)

$(EXE): $(SRC) $(INC) | $(BIN_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $(EXE) $(SRC) $(LFLAGS) 

$(BIN_DIR) :
	mkdir $(BIN_DIR)

all: $(OUTPUT) 

clean: 
	rm -f *.o $(EXE)
