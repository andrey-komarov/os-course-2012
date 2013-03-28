#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <stdio.h>

#include "malloc.h"

#define PAGE_SIZE 4096

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

// size --- без учёта себя
typedef struct free_block_t
{
    size_t size;
    struct free_block_t * next;
} free_block_t;

// size --- без учёта себя
typedef struct occupied_block_t
{
    size_t size;
    char elems[];
} occupied_block_t;

static free_block_t * head = NULL;

void print_freelist(free_block_t * block)
{
    if (block == NULL)
        printf ("[]\n");
    else
    {
        printf("at %x free %lu :: ", block, block->size);
        print_freelist(block->next);
    }
}

occupied_block_t * get_occ(void * ptr)
{
    return (occupied_block_t*)(((char*)ptr) - sizeof(occupied_block_t));
}

size_t get_size(void * ptr)
{
    return get_occ(ptr)->size;
}

// округлить вверх до кратного PAGE_SIZE
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
    occupied_block_t * res = (occupied_block_t*)(begin + new_size + sizeof(free_block_t));
    res->size = size;
    return res;
}

// Находит первый блок, в который можно утолкать size байт
free_block_t * find_large_enough(size_t size)
{
    free_block_t * current = head;
    while (current != NULL)
    {
        if (current->size >= size + sizeof(occupied_block_t) + sizeof(free_block_t))
            return current;
        current = current->next;
    }
    return NULL;
}

// выделить блок, в который влезет size байт
free_block_t * new_block(size_t size)
{
    size_t bsz = adjust(size);
    void * ptr = mmap(NULL, bsz, PROT_READ | PROT_WRITE, 
            MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    free_block_t * new = (free_block_t*)ptr;
    new->size = bsz - sizeof(free_block_t);
    new->next = NULL;
    return new;
}

void * malloc(size_t size)
{
    printf("====================================\n");
    printf("malloc %lu\n", size);
    free_block_t * current = find_large_enough(size);
    if (current == NULL)
    {
        free_block_t * new = new_block(size);
        new->next = head;
        current = head = new;
    }
    void * res = shrink(current, size)->elems;
    printf("... at %x\n", res);
    print_freelist(head);
    return res;
}

void free(void * ptr) {
    if (ptr == NULL)
        return;
    printf("====================================\n");
    printf("free %x\n", ptr);
    occupied_block_t * occ = get_occ(ptr);
    free_block_t * new_free = (free_block_t*)occ;
    new_free->size = occ->size + sizeof(occupied_block_t) - sizeof(free_block_t);
    new_free->next = head;
    head = new_free;
    print_freelist(head);
}

void * realloc(void * ptr, size_t size) {
    if (ptr == NULL)
        return NULL;
    void * ret_ptr = malloc(size);
    memcpy(ret_ptr, ptr, MIN(get_size(ptr), size));
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
    int i;
    for (i = 0; i < 10000; i++)
    {
        int * a = (int*)(malloc(sizeof(int)));
        *a = i;
    //    printf("%d ", *a);
    }
    return 0;
}

*/
