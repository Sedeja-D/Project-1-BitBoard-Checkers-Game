# Makefile for BitBoard Checkers (macOS/clang)
CC      = clang
CFLAGS  = -Wall -Wextra -std=c11 -O2
TARGET  = main
SRC     = main.c
OBJ     = $(SRC:.c=.o)

.PHONY: all run clean distclean
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

%.o: %.c bitboard.h
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)

distclean: clean
	rm -rf main.dSYM
