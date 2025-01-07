CC ?= cc
LINKER ?= cc
SIMICS_BASE ?= $(HOME)/simics/
CFLAGS=-I$(SIMICS_BASE)/src/include/simics/ -fPIC -std=c99 -Wall -Wextra -pedantic
LDFLAGS=-shared -fPIC

# All SRCDIR subdirectories that contain source files
DIRS=.

SRCDIR=src
OBJDIR=obj
SRCDIRS:=$(foreach dir, $(DIRS), $(addprefix $(SRCDIR)/, $(dir)))
OBJDIRS:=$(foreach dir, $(DIRS), $(addprefix $(OBJDIR)/, $(dir)))
SRCS:=$(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.c))
OBJS:=$(patsubst $(SRCDIR)/%, $(OBJDIR)/%, $(SRCS:.c=.o))
TARGET=../libsan-overlay.so

.PHONY: directories all main clean debug profile

all: directories main

directories:
	mkdir -p $(SRCDIRS) $(OBJDIRS)

main: $(OBJS)
	$(LINKER) $(OBJS) $(LDFLAGS) -o $(TARGET)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c $(CFLAGS) $^ -o $@

clean:
	$(RM) -- $(OBJS) $(TARGET)

debug: CFLAGS += -g -Og
debug: clean all

profile: CFLAGS += -pg
profile: LDFLAGS += -pg
profile: clean all
