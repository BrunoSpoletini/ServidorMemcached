CC=gcc
CFLAGS=-c -Wall -pthread
DIR=structures
SOURCE=server.c socket_handler.c reqHandler.c common.c utils.c ./$(DIR)/dlist.c ./$(DIR)/hash.c ./$(DIR)/stats.c ./$(DIR)/Node.c
OBJ=$(SOURCE:.c=.o)
EXE=server


.PHONY: run

run: mainExec
	./mainExec $(ARGS)

mainExec: main.c common.o utils.o $(EXE)
	$(CC) main.c common.o utils.o -o mainExec -lm

all: $(SOURCE) $(EXE) 

$(EXE): $(OBJ)
	$(CC) -pthread $(OBJ) -o $@ -lm

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(OBJ) $(EXE) mainExec server

