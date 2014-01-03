CC = gcc
CFLAGS = -Os -W -Wall -Wextra -Werror
LINK_FLAGS = -ldl

slave: slave.c
	$(CC) $(CFLAGS) -o $@ $< $(LINK_FLAGS)

slave.S: slave.c
	$(CC) $(CFLAGS) -S -o $@ $<

clean:
	git clean -xfd
