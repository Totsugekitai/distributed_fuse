SHELL := bash
.RECIPEPREFIX = >
.ONESHELL:

CFLAGS += -Wall -Wextra -g -O `pkg-config fuse3 --cflags --libs`
TARGET := dfs
SOURCES = $(wildcard ./*.c)

.PHONY: all
all:
> spack load mochi-margo@0.8.2
> make $(TARGET)

$(TARGET): $(SOURCES)
> $(CC) $(CFLAGS) -o $@ $^

.PHONY: clean
clean:
> rm -f $(TARGET)

