# Replace {FILEPATH} with the example you want to compile
CC = gcc
CFLAGS = -Wall -Wextra -std=c11
SOURCES = {FILEPATH} arena.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = program

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

.PHONY: all clean