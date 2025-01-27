CC ?= riscv64-unknown-linux-gnu-gcc
CFLAGS=-std=c99 -Wall -Wextra -pedantic -g -Og -static-libasan -fsanitize=address

TARGET=bad.o

.PHONY: all clean

all: src/bad.c
	$(CC) -c $(CFLAGS) $^ -o $(TARGET)

clean:
	$(RM) -- $(OBJS) $(TARGET)
