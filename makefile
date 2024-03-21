# Makefile for compiling project

# Compiler options
CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11

# Directories
SRCDIR_UTILITIES = utilities
SRCDIR_HUFFMAN = huffman
OBJDIR = out
BINDIR = $(OBJDIR)

# Source files
SRCS_UTILITIES = $(wildcard $(SRCDIR_UTILITIES)/*.c)
SRCS_HUFFMAN = $(wildcard $(SRCDIR_HUFFMAN)/*.c)
SRCS_MAIN = main.c

# Object files
OBJS_UTILITIES = $(patsubst $(SRCDIR_UTILITIES)/%.c,$(OBJDIR)/%.o,$(SRCS_UTILITIES))
OBJS_HUFFMAN = $(patsubst $(SRCDIR_HUFFMAN)/%.c,$(OBJDIR)/%.o,$(SRCS_HUFFMAN))
OBJS_MAIN = $(patsubst %.c,$(OBJDIR)/%.o,$(SRCS_MAIN))

# Output executable
TARGET = $(BINDIR)/main.out

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS_UTILITIES) $(OBJS_HUFFMAN) $(OBJS_MAIN)
    $(CC) $(CFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR_UTILITIES)/%.c | $(OBJDIR)
    $(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: $(SRCDIR_HUFFMAN)/%.c | $(OBJDIR)
    $(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: %.c | $(OBJDIR)
    $(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
    mkdir -p $(OBJDIR)

clean:
    $(RM) -r $(OBJDIR)

