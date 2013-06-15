#include <sys/epoll.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#include <functional>
#include <exception>
#include <iostream>
#include <vector>
#include <map>

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

struct E;
typedef std::function<void(E*, int, buffer&)> rcont;
typedef std::function<void(E*, int, buffer&)> wcont;
typedef std::function<void(void)> scont;

template<typename T>
T make();

struct E
{
    void aread(int fd, buffer& buf, rcont cont);
    void awrite(int fd, buffer& buf, wcont cont);
    void waitcycle();
};

enum class Operation
{
    IN, OUT, AC
};
auto allOperations = { Operation::IN, Operation::OUT, Operation::AC};

struct async
{
    async(int );

    int fd;
    int epfd;
    bool valid;
    async* pthis;
};

struct epollfd
{
    void subscribe(int fd, Operation op, scont cont);
    void unsubscribe(int fd, Operation op);
    void waitcycle();

    static const size_t MAXEVENTS = 10;
    epoll_event eventsbuf[MAXEVENTS];
    int epfd;
    E* root;
    map<int, decltype(epoll_event::events)> events;
    map<pair<int, Operation>, scont> actions;
};

void epollfd::subscribe(int fd, Operation op, scont cont)
{
    if (op == Operation::IN)
    {
        epoll_event e;
        auto ev = events[fd];
        if (ev & EPOLLIN)
            throw std::runtime_error("trying to add EPOLLIN event for same fd twice");
        int op = ev == 0 ? EPOLL_CTL_ADD : EPOLL_CTL_MOD;
        e.data.fd = fd;
        e.events = ev | EPOLLIN;
        epoll_ctl(epfd, op, fd, &e);
        events[fd] = e.events;
        actions[{fd, Operation::IN}] = cont;
    }
    else if (op == Operation::OUT)
    {
        epoll_event e;
        auto ev = events[fd];
        if (ev & EPOLLOUT)
            throw std::runtime_error("trying to add EPOLLOUT event for same fd twice");
        int op = ev == 0 ? EPOLL_CTL_ADD : EPOLL_CTL_MOD;
        e.data.fd = fd;
        e.events = ev | EPOLLOUT;
        epoll_ctl(epfd, op, fd, &e);
        events[fd] = e.events;
        actions[{fd, Operation::OUT}] = cont;
    }
    else if (op == Operation::AC)
    {
        throw std::runtime_error("AC is not implemented yet");
    }
}

void epollfd::unsubscribe(int fd, Operation op)
{
    if (op == Operation::IN)
    {
        epoll_event e;
        auto ev = events[fd];
        if (!(ev & EPOLLIN))
            throw std::runtime_error("No EPOLLIN while removing EPOLLIN");
        int op = ev == EPOLLIN ? EPOLL_CTL_DEL : EPOLL_CTL_MOD;
        e.data.fd = fd;
        e.events = ev &~ EPOLLIN;
        epoll_ctl(epfd, op, fd, &e);
        if (ev == EPOLLIN)
        {
            events.erase(fd);
            actions.erase({fd, Operation::IN});
        }
    }
    else if (op == Operation::OUT)
    {
        epoll_event e;
        auto ev = events[fd];
        if (!(ev & EPOLLOUT))
            throw std::runtime_error("No EPOLLOUT while removing EPOLLIN");
        int op = ev == EPOLLOUT ? EPOLL_CTL_DEL : EPOLL_CTL_MOD;
        e.data.fd = fd;
        e.events = ev &~ EPOLLOUT;
        epoll_ctl(epfd, op, fd, &e);
        if (ev == EPOLLOUT)
        {
            events.erase(fd);
            actions.erase({fd, Operation::OUT});
        }
    }
}

void epollfd::waitcycle()
{
    int n = epoll_wait(epfd, eventsbuf, MAXEVENTS, -1);
    if (n < 0)
        throw std::runtime_error("epoll_wait");
    for (int i = 0; i < n; i++)
    {
        epoll_event &e = eventsbuf[i];
        if (e.events & EPOLLIN)
            actions[{e.data.fd, Operation::IN}]();
        if (e.events & EPOLLOUT)
            actions[{e.data.fd, Operation::OUT}]();
    }
}

int main()
{
    std::function<int(int)> f = [](int a){ return a + 1;};
    auto a = 123;
    cerr << a;
}
