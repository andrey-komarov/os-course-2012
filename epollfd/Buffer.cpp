#include "Buffer.h"

#include <string.h>
#include <assert.h>

Buffer::Buffer(size_t size)
    : buf(new char[size])
    , pos(0)
    , size(0)
    , magic(MAGIC)
{}

Buffer::Buffer(Buffer&& b) noexcept
{
    *this = std::move(b);
}

Buffer& Buffer::operator=(Buffer&& b) noexcept
{
    std::swap(buf, b.buf);
    std::swap(pos, b.pos);
    std::swap(size, b.size);
    return *this;
}

Buffer::~Buffer() noexcept
{
    delete[] buf;
}

char* Buffer::readFrom()
{
    return buf;
}

char* Buffer::writeTo()
{
    return buf + pos;
}

size_t Buffer::readAvaliable() const
{
    return pos;
}

size_t Buffer::writeAvaliable() const
{
    return size - pos;
}

void Buffer::drop(size_t cnt)
{
    assert(0 <= cnt && cnt <= readAvaliable());
    memmove(buf, buf + cnt, pos - cnt);
    pos -= cnt;
}

void Buffer::peek(size_t cnt)
{
    assert(0 <= cnt && cnt <= writeAvaliable());
    pos += cnt;
}

void Buffer::check() const
{
    assert(magic == MAGIC);
}
