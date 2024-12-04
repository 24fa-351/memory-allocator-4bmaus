CC = gcc
CFLAGS = -Wall -Wextra -g

all: test_with_system_malloc test_with_custom_malloc

test_with_system_malloc: test_program.c
	$(CC) $(CFLAGS) -DSYSTEM_MALLOC -o test_with_system_malloc test_program.c

test_with_custom_malloc: malloc.c test_program.c
	$(CC) $(CFLAGS) -o test_with_custom_malloc malloc.c test_program.c

clean:
	rm -f test_with_system_malloc test_with_custom_malloc
