#include "malloc.h"

#include <iostream>
using std::cerr;

#include "Aligned.h"
#include "Allocator.h"

typedef LoggingAllocator<Allocator<LargeAligned<>>>
    MySuperMegaAllocator;

MySuperMegaAllocator alloc;

void* malloc(size_t size) noexcept
{
    return alloc.malloc(size);
}
