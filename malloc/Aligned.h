#ifndef ALIGNED_H
#define ALIGNED_H

#include <stddef.h>
#include <stdint.h>
#include <sys/mman.h>

template<size_t ALIGNMENT = (1<<16)>
struct LargeAligned
{
//  BEG_SIZ_MEM....
//          ^
public:
    void* malloc(size_t size)
    {
        size_t add = sizeof(size_t) + sizeof(size_t) + sizeof(void*) + ALIGNMENT;
        void * ptr = mmap(NULL, size + add, PROT_READ | PROT_WRITE,
                MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        void * res = reinterpret_cast<void*>((reinterpret_cast<intptr_t>(ptr) + ALIGNMENT) 
                / ALIGNMENT * ALIGNMENT);
        char * resC = reinterpret_cast<char*>(res);
        *reinterpret_cast<size_t*>(resC - sizeof(size_t)) = size + add;
        *reinterpret_cast<void**>(resC - sizeof(size_t) - sizeof(void*)) = ptr;
        return res;
    }

    void free(void* ptr)
    {
        char* ptrC = reinterpret_cast<char*>(ptr);
        void* addr = *reinterpret_cast<void**>(ptrC - sizeof(size_t) - sizeof(void*));
        size_t size = *reinterpret_cast<size_t*>(ptrC - sizeof(size_t));
        munmap(addr, size);
    }

    bool wantToMalloc(size_t size)
    {
        return size >= ALIGNMENT;
    }

    bool wantToFree(void* ptr)
    {
        intptr_t ptrI = reinterpret_cast<intptr_t>(ptr);
        return ptrI % ALIGNMENT == 0;
    }
};

#endif // ALIGNED_H
