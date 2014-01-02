slave.S: slave.c
	gcc -S -Os -W -Wall -Wextra -Werror -Wno-ignored-qualifiers -o $@ $<

clean:
	git clean -xfd
