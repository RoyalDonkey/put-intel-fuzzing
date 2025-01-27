CC ?= riscv64-unknown-linux-gnu-gcc
SIMICS_BASE ?= $(HOME)/simics/
CFLAGS=-I$(SIMICS_BASE)/src/include/simics/ -std=c99 -Wall -Wextra -pedantic

TARGET=libsan-overlay.o

.PHONY: all clean

all: src/libsan-overlay.c
	$(CC) -c $(CFLAGS) $^ -o $(TARGET)

clean:
	$(RM) -- $(TARGET)
