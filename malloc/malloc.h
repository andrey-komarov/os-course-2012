#ifndef MALLOC_H
#define MALLOC_H

#include <stddef.h>

const size_t SMALL = 32;
const size_t PAGE_SIZE = 4096;

extern "C"
{
    void* malloc(size_t size) noexcept;
}

#endif // MALLOC_H
