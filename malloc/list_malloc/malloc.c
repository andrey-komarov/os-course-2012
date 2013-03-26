#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <stdio.h>

#include "malloc.h"

#define PAGE_SIZE 4096

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

typedef struct free_block_t
{
    size_t size;
    struct free_block_t * next;
} free_block_t;

typedef struct occupied_block_t
{
    size_t size;
    char elems[];
} occupied_block_t;

static free_block_t * head = NULL;

size_t adjust(size_t size)
{
    size += sizeof(occupied_block_t) + sizeof(free_block_t);
    size = (size / PAGE_SIZE + 1) * PAGE_SIZE;
    return size;
}

// откусим в конце current себе occupied_block_t
occupied_block_t * shrink(free_block_t* current, size_t size)
{
    char * begin = (char*)current;
    size_t new_size = current->size - size - sizeof(occupied_block_t);
    current->size = new_size;
    occupied_block_t * res = (occupied_block_t*)(begin + new_size);
    res->size = size;
    return res;
}

void * malloc(size_t size)
{
    free_block_t * current = head;
    while (current != NULL)
    {
        if (current->size >= size + sizeof(occupied_block_t) + sizeof(free_block_t))
            break;
        current = current->next;
    }
    if (current != NULL)
    {
        occupied_block_t * res = shrink(current, size);
        return res->elems;
    }
    else
    {
        size_t bsz = adjust(size);
        void * ptr = mmap(NULL, bsz, PROT_READ | PROT_WRITE,
            MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        free_block_t * new_head = (free_block_t*)ptr;
        new_head->size = bsz;
        new_head->next = head;
        head = new_head;
        occupied_block_t * res = shrink(new_head, size);
        return res->elems;
    }
}

void free(void * ptr) {
    return;
}

size_t ptr_size(void * ptr) {
    size_t * size_ptr = ((size_t *) ptr) - 1;
    return *(size_ptr);
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

void crash() {
    puts("Crashed in allocator library\n");
    _exit(1);
}

int posix_memalign(void **memptr, size_t alignment, size_t size) { crash(); }
void *aligned_alloc(size_t alignment, size_t size) { crash(); }
void *valloc(size_t size) { crash(); }
void *memalign(size_t alignment, size_t size) { crash(); }
void *pvalloc(size_t size) {crash(); }


/*
int main()
{
    int ** a = (int**)(malloc(sizeof(int*) * 1000));
    int i;
    for (i = 0; i < 1000; i++)
    {
        a[i] = (int*)(malloc(sizeof(int)));
        *a[i] = i % 10;
        int j;
        for (j = 0; j <= i; j++)
            printf("%d", *a[j]);
        printf("\n");
    }
//    printf("%d", *a);
}
*/

