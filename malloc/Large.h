#ifndef LARGE_H
#define LARGE_H

template<size_t LOW>
struct Large
{
    void* malloc(size_t size)
    {
        void * ptr = mmap(NULL, size + sizeof(size_t), PROT_READ | PROT_WRITE,
                MAP_SHARED | MAP_ANONYMOUS, -1, 0);    
    }
};

#endif // LARGE_H

