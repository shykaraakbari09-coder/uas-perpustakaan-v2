CC      = gcc
CFLAGS  = -std=c11 -Wall -Wextra -pedantic -Iinclude
TARGET  = build/library
SOURCES = $(wildcard src/*.c)
HEADERS = $(wildcard include/*.h)

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	@mkdir -p build
	$(CC) $(CFLAGS) -o $@ $(SOURCES)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf build

test: build/sorttest
	./build/sorttest

build/sorttest: tests/sorttest.c src/sort.c src/fileio.c src/utils.c $(HEADERS)
	@mkdir -p build
	$(CC) $(CFLAGS) -o $@ tests/sorttest.c src/sort.c src/fileio.c src/utils.c

.PHONY: all run clean test
