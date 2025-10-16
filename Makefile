CC=gcc
CFLAGS=-std=c11 -Wall -Wextra -Werror -O2
LDFLAGS=

SRC=$(wildcard src/*.c)
OBJ=$(SRC:.c=.o)
BIN=contact_book

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(OBJ) $(BIN)

.PHONY: all clean
