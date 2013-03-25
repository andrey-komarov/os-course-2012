#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>

#define MAX_ALLOC 0x1000000 // 16 MiB

static void * buffer;
static int need_init = 1;
static size_t used = 0;

void crash() {
    puts("Crashed in allocator library\n");
    _exit(1);
}

void * malloc(size_t size) {
    if (need_init)
    {
        need_init = 0;
        buffer = mmap(NULL, MAX_ALLOC, PROT_READ | PROT_WRITE,
            MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    }
    if (size + used > MAX_ALLOC)
    {
        printf("Overflow!\n");
        crash();
    }
    void * ptr = buffer + used;
    used += size + sizeof(size_t);
    size_t * size_ptr = (size_t *) ptr;
    *(size_ptr) = size + sizeof(size_t);
    return (void *) (size_ptr + 1);
}

void free(void * ptr) {
}

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

size_t ptr_size(void * ptr) {
    size_t * size_ptr = ((size_t *) ptr) - 1;
    return *(size_ptr);
}

void * ptr_start(void * ptr) {
    size_t * size_ptr = ((size_t *) ptr) - 1;
    return (void *) size_ptr;
}

void * realloc(void * ptr, size_t size) {
    if (ptr == NULL)
        return NULL;
    void * ret_ptr = malloc(size);
    memcpy(ret_ptr, ptr, MIN(ptr_size(ptr), size));
    free(ptr);
    return ret_ptr;
}

void * calloc(size_t nmemb, size_t size) {
    void * ptr = malloc(nmemb * size);
    if (ptr == NULL)
        return NULL;
    return memset(ptr, 0, nmemb * size);
}

int posix_memalign(void **memptr, size_t alignment, size_t size) { crash(); }
void *aligned_alloc(size_t alignment, size_t size) { crash(); }
void *valloc(size_t size) { crash(); }
void *memalign(size_t alignment, size_t size) { crash(); }
void *pvalloc(size_t size) {crash(); }

