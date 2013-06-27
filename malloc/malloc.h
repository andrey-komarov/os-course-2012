#ifndef MALLOC_H
#define MALLOC_H

#include <stddef.h>

const size_t SMALL = 32;

extern "C"
{
    void* malloc(size_t size) noexcept;
}

#endif // MALLOC_H
