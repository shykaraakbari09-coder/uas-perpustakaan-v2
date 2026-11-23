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

.PHONY: all run clean
