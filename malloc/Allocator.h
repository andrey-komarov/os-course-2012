#ifndef ALLOCATOR_H
#define ALLOCATOR_H

template<typename... Args>
struct Allocatorable
{};

template<typename A, template... Args>
struct Allocator
{
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
        return left.wantToFree(size) || right.wantToFree(size);
    }

private:
    A left;
    Allocator<Args> right;
};

template<typename A>
struct Allocator : Allocatorable<A>
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
        return alloc.wantToFree(size);
    }

private:
    A alloc;
};

#endif // ALLOCATOR_H
