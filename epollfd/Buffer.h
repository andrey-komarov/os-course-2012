#ifndef BUFFER_H
#define BUFFER_H

#include <cstddef>
#include <cstdlib>
#include <algorithm>

struct Buffer
{
    explicit Buffer(size_t size);

    Buffer(const Buffer&) = delete;
    Buffer(Buffer&&) noexcept;

    Buffer& operator=(const Buffer&) = delete;
    Buffer& operator=(Buffer&&) noexcept;

    ~Buffer() noexcept;

    char* readFrom();
    char* writeTo();
    size_t readAvaliable() const;
    size_t writeAvaliable() const;
    void drop(size_t);
    void peek(size_t);
private:
    void check() const;

    char* buf;
    size_t pos;
    size_t size;
    char magic;
    static const char MAGIC = '@';
};

#endif // BUFFER_H
