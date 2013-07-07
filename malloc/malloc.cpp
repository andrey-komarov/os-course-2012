#include <stdio.h>

#include "Aligned.h"
#include "Allocator.h"

#include "malloc.h"

#include <string.h>

namespace 
{

template<typename T>
T min(const T& a, const T& b)
{
    return a < b ? a : b;
}

typedef LoggingAllocator<Allocator<LargeAligned<>>>
    MySuperMegaAllocator;

MySuperMegaAllocator alloc;

}

void* malloc(size_t size)
{
    return alloc.malloc(size);
}

void free(void* ptr)
{
    if (ptr == NULL)
        return;
//    alloc.free(ptr);
}

void* calloc(size_t nmemb, size_t size)
{
   void * ptr = alloc.malloc(nmemb * size);
   if (ptr == NULL)
       return NULL;
   return memset(ptr, 0, nmemb * size); 
}

void* realloc(void* ptr, size_t size)
{
    if (ptr == NULL)
        return NULL;
    void * ret_ptr = malloc(size);
    memcpy(ret_ptr, ptr, min(alloc.allocatedSize(ptr), size));
    free(ptr);
    return ret_ptr;
}
