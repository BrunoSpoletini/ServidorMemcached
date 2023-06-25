CC=gcc
CFLAGS=-c -Wall -pthread
DIR=structures
SOURCE=server.c socket_handler.c hash_table.c common.c 
OBJ=$(SOURCE:.c=.o)
EXE=server

all: $(SOURCE) $(EXE)
$(EXE): $(OBJ)
	$(CC) $(OBJ) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(OBJ) $(EXE)

