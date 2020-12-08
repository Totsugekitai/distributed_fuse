.RECIPEPREFIX = >

CFLAGS += -Wall -Wextra -g -O `pkg-config fuse3 --cflags --libs`

SOURCES := main.c

.PHONY: all
all: dfs

.PHONY: dfs
dfs: $(SOURCES)
> $(CC) $(CFLAGS) -o $@ $+

.PHONY: clean
clean:
> rm -f dfs

