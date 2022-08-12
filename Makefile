CC=gcc
CFLAGS=-Wall -pedantic -g
LDLIBS=-lparse_json -lm

all: split_json ip_match graph partition

%: %.o

clean:
	rm split_json ip_match graph partition
