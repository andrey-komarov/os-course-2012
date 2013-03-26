#ifndef __my_malloc__
#define __my_malloc__

#include <unistd.h>

void * malloc(size_t size);
void free(void * ptr);
void * realloc(void * ptr, size_t size);
void * calloc(size_t nmemb, size_t size);

int posix_memalign(void **memptr, size_t alignment, size_t size);
void *aligned_alloc(size_t alignment, size_t size);
void *valloc(size_t size);
void *memalign(size_t alignment, size_t size);
void *pvalloc(size_t size);

#endif // __my_malloc__
