CXX=gcc
CFLAGS=-Wall -pedantic -g
LFLAGS=-lparse_json -lm

all: split_json.out split_json_moodle.out split_json_noid.out ip_match.out graph.out partition.out

%.out: %.o
	$(CXX) -o $@ $^ $(LFLAGS)

%.o: %.c
	$(CXX) -c $(CFLAGS) $^ -o $@

clean:
	rm *.out *.o
