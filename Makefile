CC=gcc
CFLAGS=-Wall -pedantic -g
LDLIBS=-lparse_json -lm

all: split_json split_json_moodle split_json_noid ip_match graph partition

%: %.o

clean:
	rm split_json split_json_moodle split_json_noid ip_match graph partition
