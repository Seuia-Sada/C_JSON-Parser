
#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include <memory.h>

typedef struct Stack
{
    uint_fast8_t storage_size;
    size_t  size;
    union {
        void* volatile index;
    };
}   Stack;

Stack* Stack_create(uint8_t const storage_size)
{
    Stack* ret = { malloc(sizeof(Stack)) };

    ret->storage_size = storage_size;
    ret->size  = 0;
    ret->index = NULL;

    return ret;
}

void Stack_clear(Stack* restrict const ptr)
{
    ptr->storage_size = 0;
    ptr->size = 0;

    if (ptr->index)
        free(ptr->index);

    ptr->index = NULL;
}

void Stack_destroy(Stack* restrict const ptr)
{
    Stack_clear(ptr);

    free(ptr);
}

void Stack_storage_resize(Stack* restrict ptr, uint8_t const new_storage_size) {
    if (0 == new_storage_size)
        return Stack_clear(ptr);

    if (new_storage_size == ptr->storage_size)
        return;

    if (! ptr->size)
        return (void) (ptr->storage_size = new_storage_size);

    void* const new_index = { calloc(1, (new_storage_size) * ptr->size) };

    void* write = { new_index };
    void* read  = { ptr->index };

    size_t const storage_copy_size = (0 < new_storage_size - ptr->storage_size)
    ?   ptr->storage_size
    :   new_storage_size;

    for (size_t i = 0; i < ptr->size; ++i) {
        memcpy(write, read, storage_copy_size);

        write += new_storage_size;
        read  += ptr->storage_size;
    }

    free(ptr->index);

    ptr->storage_size = new_storage_size;
    ptr->index = new_index;
}

void* Stack_push(Stack* const ptr, void const* const memory_address)
{
    size_t const item_memory_distance = ptr->storage_size * ++ptr->size;

    ptr->index = realloc(ptr->index, item_memory_distance);

    void* push = { ptr->index + item_memory_distance - ptr->storage_size };

    memcpy(push, memory_address, ptr->storage_size);

    return push;
}

void* Stack_pop(Stack* const ptr)
{
    switch (ptr->size) {
        void* ret;

    case 1:
        ret = { ptr->index };

        ptr->size  = 0;
        ptr->index = NULL;
    case 0:
        return ret;

    default:
        {
            size_t const item_memory_distance = ptr->storage_size * --ptr->size;

            ret = { memcpy(malloc(ptr->storage_size), ptr->index + item_memory_distance, ptr->storage_size) };

            ptr->index = realloc(ptr->index, item_memory_distance);
        }

        return ret;
    }
}

