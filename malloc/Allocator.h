#ifndef ALLOCATOR_H
#define ALLOCATOR_H

template<typename... Args>
struct Allocatorable
{};

template<typename A, typename... Args>
struct Allocator
{
private:
    A left;
    Allocator<Args...> right;
public:

    void* malloc(size_t size)
    {
        if (left.wantToMalloc(size))
            return left.malloc(size);
        return right.malloc(size);
    }

    void free(void* ptr)
    {
        if (left.wantToFree(ptr))
            left.free(ptr);
        right.free(ptr);
    }

    bool wantToMalloc(size_t size)
    {
        return left.wantToMalloc(size) || right.wantToMalloc(size);
    }

    bool wantToFree(void* ptr)
    {
        return left.wantToFree(ptr) || right.wantToFree(ptr);
    }

    size_t allocatedSize(void* ptr)
    {
        if (left.wantToFree(ptr))
            return left.allocatedSize(ptr);
        else
            return right.allocatedSize(ptr);
    }
};

template<typename A>
struct Allocator<A>
{
    void* malloc(size_t size)
    {
        return alloc.malloc(size);
    }

    void free(void* ptr)
    {
        alloc.free(ptr);
    }

    bool wantToMalloc(size_t size)
    {
        return alloc.wantToMalloc(size);
    }

    bool wantToFree(void* ptr)
    {
        return alloc.wantToFree(ptr);
    }

    size_t allocatedSize(void* ptr)
    {
        return alloc.allocatedSize(ptr);
    }

private:
    A alloc;
};

template<typename Alloc>
struct LoggingAllocator
{
private:
    typedef Allocator<Alloc> A;
    A alloc;
public:

    void* malloc(size_t size)
    {
        fprintf(stderr, "malloc(%ld) = ", size);
        void* res = alloc.malloc(size);
        fprintf(stderr, "%lx\n", reinterpret_cast<intptr_t>(res));
        return res;
    }

    void free(void* ptr)
    {
        fprintf(stderr, "free(%lx)\n", reinterpret_cast<intptr_t>(ptr));
        alloc.free(ptr);
    }

    bool wantToFree(void* ptr)
    {
        return alloc.wantToFree(ptr);
    }

    bool wantToMalloc(void* ptr)
    {
        return alloc.wantToFree(ptr);
    }

    size_t allocatedSize(void* ptr)
    {
        return alloc.allocatedSize(ptr);
    }

};

#endif // ALLOCATOR_H
