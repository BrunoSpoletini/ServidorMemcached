CC=gcc
CFLAGS=-c -Wall -pthread
DIR=structures
SOURCE=server.c socket_handler.c hash_table.c common.c utils.c
OBJ=$(SOURCE:.c=.o)
EXE=server

run: mainExec
	./mainExec

mainExec: main.c common.o utils.o $(EXE)
	$(CC) main.c common.o utils.o -o mainExec

all: $(SOURCE) $(EXE) 

$(EXE): $(OBJ)
	$(CC) $(OBJ) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(OBJ) $(EXE) mainExec

