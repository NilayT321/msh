OUTPUT = msh
CC = gcc
CFLAGS = -g -Wall -fsanitize=address -std=c99 
LFLAGS = -lm


$(OUTPUT): $(OUTPUT).c devutils.c shellutils.c
	$(CC) $(CFLAGS) -o $(OUTPUT) *.c $(LFLAGS) 

all: $(OUTPUT) 

clean: 
	rm -f *.o $(OUTPUT)
