#include "malloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct FreeBlock {
    size_t size;
    struct FreeBlock* next;
} FreeBlock;

static void* memory_pool = NULL;
static FreeBlock* free_list = NULL;
static const size_t BLOCK_HEADER_SIZE = sizeof(size_t);
static const size_t ALIGNMENT = 8;

void* get_me_blocks(ssize_t how_much) {
    void* ptr = sbrk(0);
    if (sbrk(how_much) == (void*)-1) {
        return NULL;
    }
    return ptr;
}

static size_t align_size(size_t size) {
    return (size + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1);
}

void* my_malloc(size_t size) {
    if (size == 0) return NULL;
    size = align_size(size + BLOCK_HEADER_SIZE);

    FreeBlock** current = &free_list;
    while (*current) {
        if ((*current)->size >= size) {
            void* allocated_block = (void*)(*current);
            size_t block_size = (*current)->size;

            if (block_size > size + sizeof(FreeBlock)) {
                // Split block
                FreeBlock* new_block = (FreeBlock*)((char*)(*current) + size);
                new_block->size = block_size - size;
                new_block->next = (*current)->next;
                *current = new_block;
            } else {
                // Use entire block
                *current = (*current)->next;
            }

            *(size_t*)allocated_block = size;
            memset((char*)allocated_block + BLOCK_HEADER_SIZE, 0, size - BLOCK_HEADER_SIZE);
            return (char*)allocated_block + BLOCK_HEADER_SIZE;
        }
        current = &((*current)->next);
    }

    // No suitable block, expand memory
    void* new_block = get_me_blocks(size);
    if (!new_block) return NULL;

    *(size_t*)new_block = size;
    memset((char*)new_block + BLOCK_HEADER_SIZE, 0, size - BLOCK_HEADER_SIZE);
    return (char*)new_block + BLOCK_HEADER_SIZE;
}

void my_free(void* ptr) {
    if (!ptr) return;

    void* block = (char*)ptr - BLOCK_HEADER_SIZE;
    size_t block_size = *(size_t*)block;

    FreeBlock* free_block = (FreeBlock*)block;
    free_block->size = block_size;

    free_block->next = free_list;
    free_list = free_block;
}

void* my_realloc(void* ptr, size_t size) {
    if (!ptr) return my_malloc(size);
    if (size == 0) {
        my_free(ptr);
        return NULL;
    }

    void* block = (char*)ptr - BLOCK_HEADER_SIZE;
    size_t old_size = *(size_t*)block - BLOCK_HEADER_SIZE;

    if (size <= old_size) return ptr;

    void* new_block = my_malloc(size);
    if (new_block) {
        memcpy(new_block, ptr, old_size);
        my_free(ptr);
    }
    return new_block;
}

void print_heap_status(void) {
    FreeBlock* current = free_list;
    printf("Free blocks:\n");
    while (current) {
        printf(" - Block at %p, size: %zu\n", current, current->size);
        current = current->next;
    }
}
