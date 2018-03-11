CC=gcc
CFLAGS = -Wall -pedantic

src_files = $(wildcard src/*.c) $(wildcard src/actions/*.c)
src_objects = $(src_files:%.c=%.o)

name = mtg-client

objects = $(src_objects)

$(name): $(objects)
	$(CC) -o $(name) $(objects) -lncurses -lmenu -ltinfo -lform -lcurl

debug: $(name)
	valgrind --leak-check=full ./$(name)
clean:
	rm -f $(objects)
