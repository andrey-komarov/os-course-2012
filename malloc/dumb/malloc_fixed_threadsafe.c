#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>

#include <pthread.h>

#define MAX_ALLOC 0x4000000 // 64 MiB

static char buffer[MAX_ALLOC];
static size_t used = 0;
static pthread_mutex_t mmalloc = PTHREAD_MUTEX_INITIALIZER;

void crash() {
    puts("Crashed in allocator library\n");
    _exit(1);
}

// Хватаем из буфера очередной кусок размера size + sizeof(size_r), в начало
// пишем эту сумму --- длину блока, хвост отдаём. Длина нужна для realloc-а
//
// При освобождении халтурим и ничего не делаем
//
// Только полная блокировка, только хардкор!
void * malloc(size_t size) {
    if (size == 0)
        return NULL;
    pthread_mutex_lock(&mmalloc);
    if (size + used > MAX_ALLOC)
    {
        puts("Out of memory :(\n");
        crash();
    }
    void * ptr = (void*)(buffer + used);
    used += size + sizeof(size_t);
    size_t * size_ptr = (size_t *) ptr;
    *(size_ptr) = size + sizeof(size_t);
    pthread_mutex_unlock(&mmalloc);
    return (void *) (size_ptr + 1);
}

void free(void * ptr) {
}

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

size_t ptr_size(void * ptr) {
    size_t * size_ptr = ((size_t *) ptr) - 1;
    return *(size_ptr) - sizeof(size_t);
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

