SHELL := bash
.RECIPEPREFIX = >
.ONESHELL:

CFLAGS += -Wall -Wextra -g -O `pkg-config fuse3 margo --cflags --libs` -lpthread
TARGET_CLIENT := dfs_client
SRC_CLIENT = $(wildcard client/*.c) $(wildcard common/*.c)
TARGET_SERVER := dfs_server
SRC_SERVER = $(wildcard server/*.c) $(wildcard common/*.c)

.PHONY: all
all:
> spack load mochi-margo@0.8.2
> make $(TARGET_CLIENT)
> make $(TARGET_SERVER)

$(TARGET_CLIENT): $(SRC_CLIENT)
> $(CC) $(CFLAGS) -o $@ $^

$(TARGET_SERVER): $(SRC_SERVER)
> $(CC) $(CFLAGS) -o $@ $^

.PHONY: clean
clean:
> rm -f $(TARGET_CLIENT) $(TARGET_SERVER)

