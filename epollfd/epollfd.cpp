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

struct epollfd
{
    epollfd(int epfd, int fd, int events, std::function<void()>);
    epollfd(epollfd&&);
    epollfd& operator=(epollfd&&);
    ~epollfd();

    int epfd, fd;
    epoll_event e;
    std::function<void()> f;
private:
    epollfd(const epollfd&);
    epollfd& operator=(const epollfd&);
};

epollfd::epollfd(int epfd, int fd, int events, std::function<void()> f)
    : epfd(epfd), fd(fd), f(f)
{
    e.events = events;
    e.data.ptr = &this->f;
    epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &e);
}

epollfd::~epollfd()
{
    if (epfd < 0)
        return;
    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &e);
}

epollfd::epollfd(epollfd&& r)
{

}

struct E;
typedef std::function<void(E&, int, buffer&)> acont;

struct E
{
    E();
    ~E();

    void accept(int fd, std::function<void(int)> cont);
    void aread(int fd, buffer& buf, acont cont);
    void awrite(int fd, buffer& buf, acont cont);
    void mainloop();

    int efd;

private:
    E(const E&);
    E& operator=(const E&);
    E& operator=(E&&);
    E(E&&);
};

E::E()
{
    efd = epoll_create(1);
    if (efd < 0)
        throw std::runtime_error("epoll_create");
}

E::~E()
{
    if (close(efd) < 0)
        throw std::runtime_error("close(efd)");
}

int main()
{
    E e;
    std::function<int(int)> f = [](int a){ return a + 1;};
    auto a = 123;
    cerr << a;
}
