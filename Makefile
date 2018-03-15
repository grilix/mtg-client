CC=gcc
CFLAGS = -Wall -pedantic

src_files = $(wildcard src/*.c) $(wildcard src/chest/actions/*.c) \
	    $(wildcard src/bplib/*.c) $(wildcard src/chest/*c) \
	    $(wildcard src/chest/client/*.c) $(wildcard src/json/*.c)
src_objects = $(src_files:%.c=%.o)

name = mtg-client

objects = $(src_objects)

$(name): $(objects)
	$(CC) -o $(name) $(objects) -lncurses -lmenu -ltinfo -lform -lcurl -lm

debug: $(name)
	valgrind --leak-check=full ./$(name)
clean:
	rm -f $(objects)
