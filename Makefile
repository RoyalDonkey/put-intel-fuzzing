CC ?= riscv64-unknown-linux-gnu-gcc
LD ?= riscv64-unknown-linux-gnu-ld

BAD_PATCHED_O = bad_patched.o
TARGET = bad

.PHONY: all clean

all: bad

bad:
	$(MAKE) -C libsan-overlay
	$(MAKE) -C bad-program
	$(LD) -o $(BAD_PATCHED_O) -r bad-program/bad.o libsan-overlay/libsan-overlay.o
	$(CC) -o $(TARGET) $(BAD_PATCHED_O) -static-libasan -fsanitize=address
	$(RM) $(BAD_PATCHED_O)

clean:
	$(MAKE) -C libsan-overlay clean
	$(MAKE) -C bad-program clean
	$(RM) $(TARGET) $(BAD_PATCHED_O)
