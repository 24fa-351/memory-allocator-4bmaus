#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef SYSTEM_MALLOC
#define my_malloc malloc
#define my_free free
#else
#include "malloc.h"
#endif

#define TEST_SIZE 30

int rand_between(int min, int max) {
    return rand() % (max - min + 1) + min;
}

#define MIN(a, b) ((a) < (b) ? (a) : (b))

int main(int argc, char *argv[]) {
    (void)argc; // Suppress unused parameter warning
    (void)argv; // Suppress unused parameter warning

    srand(12345); // Fixed seed for reproducibility

    char *test_string = "Now is the time for all good people to come to the aid of their country.";
    char *ptrs[TEST_SIZE];

    for (int i = 0; i < TEST_SIZE; i++) {
        int size = rand_between(1, strlen(test_string) + 1);
        printf("\n[%d] Allocating %d bytes\n", i, size);

        ptrs[i] = my_malloc(size);
        if (ptrs[i] == NULL) {
            printf("[%d] Allocation failed\n", i);
            return 1;
        }

        int len_to_copy = MIN((int)strlen(test_string), size - 1);
        strncpy(ptrs[i], test_string, len_to_copy);
        ptrs[i][len_to_copy] = '\0';

        printf("[%d] Allocated at %p: '%s'\n", i, (void*)ptrs[i], ptrs[i]);

        // Randomly free one of the previously allocated blocks
        if (i > 0 && rand_between(0, 1)) {
            int free_index = rand_between(0, i - 1);
            if (ptrs[free_index]) {
                printf("[%d] Freeing block at %p\n", free_index, (void*)ptrs[free_index]);
                my_free(ptrs[free_index]);
                ptrs[free_index] = NULL;
            }
        }
    }

    // Clean up remaining allocations
    for (int i = 0; i < TEST_SIZE; i++) {
        if (ptrs[i]) {
            printf("[%d] Freeing block at %p\n", i, (void*)ptrs[i]);
            my_free(ptrs[i]);
            ptrs[i] = NULL;
        }
    }

#ifndef SYSTEM_MALLOC
    // Debug: Print the heap status
    print_heap_status();
#endif

    return 0;
}
