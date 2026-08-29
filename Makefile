.PHONY: all clean test
.SECONDARY:

CC = gcc
CFLAGS += -std=c89 -O2 -s -Iinclude
CFLAGS += -Wall -Wextra -pedantic

RAWS = arbmath arith encode random sha512 ed25519
SRCS = $(addprefix src/,$(addsuffix .c,$(RAWS)))
OBJS = $(addprefix bin/,$(addsuffix .o,$(RAWS)))

TESTS = add_mod_p sub_mod_p mul_mod_p pow_mod_p mod_order \
        mul_256to512 sha512 add_point times_point verify
TEXES = $(addprefix bin/tests/,$(TESTS))

# Named targets

all: lib/libed25519.a $(TEXES)

clean:
	rm -rf bin lib

test: $(TESTS)

# Library build

lib/libed25519.a: $(OBJS) | lib
	ar rcs $@ $^
	ranlib $@

bin/%.o: src/%.c | bin
	$(CC) -c -o $@ $< $(CFLAGS)

bin:
	mkdir bin

lib:
	mkdir lib

# Tests build

%: bin/tests/% tests/tv/%.txt
	@begin=$$(date +%s) && \
	cd tests && ../$<   && \
	echo "--> $$(($$(date +%s)-begin)) seconds"

bin/tests/%: tests/%.c tests/out.c lib/libed25519.a | bin/tests
	$(CC) -o $@ $< tests/out.c $(CFLAGS) -Llib -led25519

tests/tv/%.txt: tests/tv/%.py
	python3 $< > $@

bin/tests: | bin
	mkdir bin/tests
