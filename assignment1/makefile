CC = gcc
CFLAGS = -Wall
LIBS = -fopenmp
SERIAL_CODE =bloom.c
PARALLEL_CODE =bloom_parallel.c
READ_FILE = read_file.c

all: bloom_parallel bloom_serial

bloom_parallel: $(PARALLEL_CODE) $(READ_FILE)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

bloom_serial: $(SERIAL_CODE) $(READ_FILE)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f bloom_parallel bloom_serial

.PHONY: all clean