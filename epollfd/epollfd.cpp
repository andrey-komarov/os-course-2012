#include <sys/epoll.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#include <functional>
#include <exception>
#include <iostream>
#include <vector>
#include <map>
#include <set>

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

    char* begin()
    {
        return buf + pos;
    }
};

struct E;
struct epollfd;
struct async;
typedef std::function<void(E*, int fd, buffer&, int rd)> rcont;
typedef std::function<void(E*, int fd, buffer&, int wr)> wcont;
typedef std::function<void(void)> scont;

template<typename T>
T make();

enum class Operation
{
    IN, OUT, AC
};
auto allOperations = { Operation::IN, Operation::OUT, Operation::AC};

struct epollfd
{
    epollfd(E* root);
    epollfd(const epollfd&) = delete;
    epollfd& operator=(const epollfd&) = delete;
    epollfd& operator=(epollfd&&) = delete;
    epollfd(epollfd&&) = delete;
    ~epollfd();

    void subscribe(int fd, Operation op, scont cont);
    void unsubscribe(int fd, Operation op);
    void waitcycle();

    static const size_t MAXEVENTS = 10;
    epoll_event eventsbuf[MAXEVENTS];
    int epfd;
    E* root;
    map<int, decltype(epoll_event::events)> events;
    map<pair<int, Operation>, scont> actions;
    set<async> alive;
};

struct async
{
    async();
    async(Operation op, scont cont);
    ~async();

    void invalidate();
    bool isValid() const;

    bool operator<(const async&) const;
    bool operator==(const async&) const;

    int fd;
    int epfd;
    bool valid;
    E* root;
    async* pthis;
    scont cont;
};

struct E
{
    E();
    ~E();

    void aread(int fd, buffer& buf, rcont cont);
    void awrite(int fd, buffer& buf, wcont cont);
    void waitcycle();

    epollfd epfd;
};

async::async()
    : valid(false)
    , pthis(new async)
{}

bool async::isValid() const
{
    return valid;
}

async::invalidate()
{
    valid = false;
    fd = -1;
}

async::~async()
{
    if (isValid())
    {
        
    }
    delete pthis;
}

bool async::operator<(const async& a) const
{
    if (fd != a.fd)
        return fd < a.fd;
    if (epfd != a.epfd)
        return epfd < a.epfd;
    if (valid != a.valid)
        return valid;
    return reinterpret_cast<intptr_t>(pthis) < reinterpret_cast<intptr_t>(a.pthis);
}

bool async::operator==(const async& a) const
{
    if (fd != a.fd)
        return false;
    if (epfd != a.epfd)
        return false;
    if (valid != a.valid)
        return false;
    return reinterpret_cast<intptr_t>(pthis) == reinterpret_cast<intptr_t>(a.pthis);
}




E::E()
    : epfd(this)
{}

E::~E()
{
}

void E::aread(int fd, buffer& buf, rcont cont)
{
    scont newCont = [fd, &buf, &cont, this]()
    {
        int n = read(fd, buf.begin(), buf.freeSize());
        cont(this, fd, buf, n);
    };
    
}

epollfd::epollfd(E* root)
    : root(root)
{}

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
        int fd = e.data.fd;
        if (e.events & EPOLLIN)
            actions[{fd, Operation::IN}]();
        if (e.events & EPOLLOUT)
            actions[{fd, Operation::OUT}]();
    }
}

epollfd::~epollfd()
{
    for (auto p : events)
    {
        if (p.second)
        {
            epoll_event tmp;
            int e = epoll_ctl(epfd, EPOLL_CTL_DEL, p.first, &tmp);
            if (e < 0)
                throw std::runtime_error("~epollfd::clear_epoll");
        }
    }
    close(epfd);
}

int main()
{
    std::function<int(int)> f = [](int a){ return a + 1;};
    auto a = 123;
    cerr << a;
}
