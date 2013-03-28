#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <stdio.h>

#include <assert.h>

#include "malloc.h"

#define PAGE_SIZE 4096

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define BLK_FREE 0x1
#define BLK_BOUND 0x2

/* Что происходит?
 *
 * Есть суперблоки. В каждом суперблоке лежат блоки. Самый левый и самый правый
 * блоки суперблока --- служебные. По блоку можно получить левого и правого 
 * соседей.
 *
 * Свободные блоки объединены в двусвязный список. Когда просят malloc,
 * в списле ищется первый, имеющий достаточный размер. Когда делают free, у 
 * блока смотрятся соседи, и, если они свободны, то они выпиливаются из списка, 
 * а сосед сливается с удаляемым. 
 */ 

typedef struct block_t
{
    struct block_t * prev;
    struct block_t * next;
    struct block_t * prev_in_superblock;
    size_t size; // Чистый размер. Вот столько можно дать памяти
    int flags;
    char elems[];
} block_t;

static block_t * head = NULL;

void set_head(block_t * block)
{
    block->next = head;
    head = block;
    head->prev = NULL;
}

void remove_from_list(block_t* block)
{
    if (block->prev != NULL)
        block->prev->next = block->next;
    if (block->next != NULL)
        block->next->prev = block->prev;
    if (block == head)
        head = block->next;
}

block_t * init_bounding_block(void * ptr)
{
    block_t * block = (block_t*) ptr;
    block->flags = BLK_BOUND;
    return block;
}

block_t * new_superblock(size_t size)
{
    assert (size > 3 * sizeof(block_t));
    void * ptr = mmap(NULL, size, PROT_READ | PROT_WRITE,
            MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    char * cptr = (char*)ptr;
    block_t * left_bound = init_bounding_block(ptr);
    init_bounding_block(cptr + size - sizeof(block_t));
    block_t * main_block = (block_t*)(cptr + sizeof(block_t));
    main_block->prev = NULL;
    main_block->next = NULL;
    main_block->size = size - 3 * sizeof(block_t);
    main_block->prev_in_superblock = left_bound;
    main_block->flags = BLK_FREE;
    return main_block;
}

block_t * next_in_superblock(block_t * block)
{
    char* ptr = (char*)block;
    return (block_t*)(ptr + sizeof(block_t) + block->size);
}

void print_my_superblock(block_t * block)
{
    block_t * cur = block;
    while (cur->prev_in_superblock != NULL)
        cur = cur->prev_in_superblock;
    cur = next_in_superblock(cur);
    printf("(%lx)[", (size_t)block);
    while (!(cur->flags & BLK_BOUND))
    {
        if (cur->flags & BLK_FREE)
            printf("F %lu; ", cur->size);
        else
            printf("O %lu; ", cur->size);
        cur = next_in_superblock(cur);
    }
    printf("]\n");
}

void print_freelist()
{
    printf("free[");
    block_t * cur = head;
    while (cur != NULL)
    {
        printf("%lu ", cur->size);
        cur = cur->next;
    }
    printf("]\n");
}

size_t adjust(size_t size)
{
    size += 3 * sizeof(block_t);
    size = (size / PAGE_SIZE + 1) * PAGE_SIZE;
    return size;
}

void attach(block_t * block)
{
    if (head != NULL)
        head->prev = block;
    set_head(block);
}

// Откусить от блока кусок, достаточный для выделения size байт и положить его
// в голову списка
void shrink(block_t * block, size_t size)
{
    // Если можем разбить на две части... 
    if (block->size > size + sizeof(block_t))
    {
        remove_from_list(block);
        char * begin1 = (char*)block;
        char * begin2 = begin1 + sizeof(block_t) + size;
        block_t * b1 = (block_t*)begin1;
        block_t * b2 = (block_t*)begin2;
        b1->prev_in_superblock = block->prev_in_superblock;
        b2->prev_in_superblock = b1;
        b1->flags = BLK_FREE;
        b2->flags = BLK_FREE;
        b2->size = block->size - sizeof(block_t) - size;
        b1->size = size;
        set_head(b2);
        set_head(b1);
    }
    else // Иначе --- отдаём целиком
    {
        remove_from_list(block);
        set_head(block);
    }
}

block_t* block_by_addr(void* ptr)
{
    char* cptr = (char*)ptr;
    return (block_t*)(cptr - sizeof(block_t));
}

block_t * find_large_enough(size_t size)
{
    block_t * cur = head;
    while (cur != NULL)
    {
        if (cur->size >= size)
            return cur;
        cur = cur->next;
    }
    return NULL;
}

void * malloc(size_t size)
{
    block_t * cur = find_large_enough(size);
    if (cur != NULL)
    {
        remove_from_list(cur);
        set_head(cur);
    }
    else
    {
        block_t * new = new_superblock(adjust(size));
        set_head(new);
        cur = head;
    }
    shrink(head, size);
    block_t* res = head;
    remove_from_list(head);
    res->flags = 0;
    return res->elems;
}

void free(void * ptr)
{
}

void * realloc(void * ptr, size_t size) {
    if (ptr == NULL)
        return NULL;
    void * ret_ptr = malloc(size);
    memcpy(ret_ptr, ptr, MIN(block_by_addr(ptr)->size, size));
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

/*
int posix_memalign(void **memptr, size_t alignment, size_t size) { crash(); }
void *aligned_alloc(size_t alignment, size_t size) { crash(); }
void *valloc(size_t size) { crash(); }
void *memalign(size_t alignment, size_t size) { crash(); }
void *pvalloc(size_t size) {crash(); }
*/

int main()
{
    malloc(1);
    malloc(22);
    malloc(1230);
    malloc(10000);
    print_my_superblock(head);
    print_freelist();
    return 0;
}
