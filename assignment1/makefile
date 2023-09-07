CC = gcc
CFLAGS = -Wall
LIBS = -fopenmp
SOURCE_CODE =bloom.c
READ_FILE = read_file.c

all: bloom_parallel bloom_serial

bloom_parallel: $(SOURCE_CODE) $(READ_FILE)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

bloom_serial: $(SOURCE_CODE) $(READ_FILE)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f bloom_parallel bloom_serial

.PHONY: all clean