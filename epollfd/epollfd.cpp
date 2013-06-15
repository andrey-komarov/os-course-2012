#include <sys/epoll.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#include <functional>
#include <exception>
#include <iostream>
#include <vector>

using namespace std;

struct buffer
{
    static const size_t SIZE = 10;
    
    char buf[SIZE];
    size_t pos;

    size_t freeSize() const
    {
        return SIZE - pos;
    }

    void drop(size_t size)
    {
        assert (size <= pos);
        memmove(buf, buf + size, pos);
        pos -= size;
    }

    bool isFull() const
    {
        return pos == SIZE;
    }
};

struct epollfd;
typedef std::function<void(epollfd&, int, buffer&)> acont;

struct epollfd
{
    epollfd();
    ~epollfd();

    void accept(int fd, std::function<void(int)> cont);
    void aread(int fd, buffer& buf, acont cont);
    void awrite(int fd, buffer& buf, acont cont);
    void mainloop();

    int efd;

private:
    epollfd(const epollfd&);
    epollfd& operator=(const epollfd&);
    epollfd& operator=(epollfd&&);
    epollfd(epollfd&&);
};

epollfd::epollfd()
{
    efd = epoll_create(1);
    if (efd < 0)
        throw std::runtime_error("epoll_create");
}

epollfd::~epollfd()
{
    if (close(efd) < 0)
        throw std::runtime_error("close(efd)");
}

int main()
{
    epollfd e;
    std::function<int(int)> f = [](int a){ return a + 1;};
    auto a = 123;
    cerr << a;
}
