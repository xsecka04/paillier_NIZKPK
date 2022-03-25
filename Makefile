CC = gcc

LIBFLAGS = -lgmp -lssl -lcrypto

LDFLAGS = -L/usr/local/opt/openssl@3/lib
CFLAGS = -I/usr/local/opt/openssl@3/include

SOURCES = src/nizkpk_join.c
HEADERS = lib/nizkpk_join.h

all: test join test_serialized

test:
	$(CC) $(CFLAGS) $(SOURCES) $(LDFLAGS) test.c -o $@ $(LIBFLAGS)

test_serialized:
	$(CC) $(CFLAGS) $(SOURCES) $(LDFLAGS) test_serialized.c -o $@ $(LIBFLAGS)

join:
	$(CC) $(CFLAGS) $(SOURCES) $(LDFLAGS) join.c -o $@ $(LIBFLAGS)


.PHONY: clean

clean:
	rm -f test
	rm -f join
	rm -f test_serialized
	rm -f *.json