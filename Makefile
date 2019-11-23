CC = gcc

DEFS = -D_DEFAULT_SOURCE -D_POSIX_C_SOURCE=200809L
CFLAGS = -Wall -g -std=c99 -pedantic $(DEFS)

OBJECTS = main.o term.o compare_term.o variable_term.o sort_term.o simplify_term.o

.PHONY: compile clean
compile: algebra-system

algebra-system: $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

main.o: main.c
term.o: term.c
compare_term.o: compare_term.c
variable_term.o: variable_term.c
sort_term.o: sort_term.c
simplify_term.o: simplify_term.c

clean:
	rm -rf *.o algebra-system

# compile: points to the targets to be built by default
# clean: remove all files produced during the build process
